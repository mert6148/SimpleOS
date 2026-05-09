"""
lx-stack_depot_lookup

Bu script Linux kernel gdb yardımcı komutudur. SimpleOS projesinde doğrudan
kullanılmasa da `script/include/` altında faydalı bir debug aracı olarak
saklanır. GDB dışındaki ortamlarda import edilirse sessizce no-op olur.
"""

from __future__ import annotations

from typing import Tuple, Any

try:
    import gdb  # type: ignore
    from linux import utils, constants  # type: ignore
except Exception:  # pragma: no cover
    gdb = None
    utils = None
    constants = None


def _require_gdb() -> None:
    if gdb is None or utils is None or constants is None:
        raise RuntimeError("Bu modül sadece GDB (linux kernel scripts) içinde çalışır.")


def _help() -> None:
    _require_gdb()
    text = """Usage: lx-stack_depot_lookup [Hex handle value]
Example:
  lx-stack_depot_lookup 0x00c80300
"""
    raise gdb.GdbError(text)


def _stack_depot_fetch(handle: Any) -> Tuple[Any, int]:
    _require_gdb()

    if not constants.LX_CONFIG_STACKDEPOT:
        raise gdb.GdbError("CONFIG_STACKDEPOT is not enabled")

    stack_record_type = utils.CachedType("struct stack_record")
    depot_stack_align = 4

    stack_depot_disabled = gdb.parse_and_eval("stack_depot_disabled")
    if stack_depot_disabled:
        raise gdb.GdbError("stack_depot_disabled")

    if int(handle) == 0:
        raise gdb.GdbError("handle is 0")

    handle_parts_t = gdb.lookup_type("union handle_parts")
    parts = handle.cast(handle_parts_t)
    offset = parts["offset"] << depot_stack_align

    pools_num = gdb.parse_and_eval("pools_num")
    pool_index = parts["pool_index_plus_1"] - 1

    if pool_index >= pools_num:
        gdb.write(
            "pool index %d out of bounds (%d) for stack id 0x%08x\n"
            % (parts["pool_index"], pools_num, handle)
        )
        return gdb.Value(0), 0

    stack_pools = gdb.parse_and_eval("stack_pools")
    try:
        pool = stack_pools[pool_index]
        stack = (
            pool + gdb.Value(offset).cast(utils.get_size_t_type())
        ).cast(stack_record_type.get_type().pointer())
        size = int(stack["size"].cast(utils.get_ulong_type()))
        return stack["entries"], size
    except Exception as e:
        gdb.write("%s\n" % e)
        return gdb.Value(0), 0


def _stack_depot_print(handle: Any) -> None:
    _require_gdb()
    entries, nr_entries = _stack_depot_fetch(handle)
    for i in range(0, nr_entries):
        try:
            gdb.execute("x /i 0x%x" % (int(entries[i])))
        except Exception as e:
            gdb.write("%s\n" % e)


if gdb is not None:

    class StackDepotLookup(gdb.Command):
        """Search backtrace by handle"""

        def __init__(self) -> None:
            if constants.LX_CONFIG_STACKDEPOT:
                super().__init__("lx-stack_depot_lookup", gdb.COMMAND_SUPPORT)

        def invoke(self, args: str, from_tty: bool) -> None:
            if not constants.LX_CONFIG_STACKDEPOT:
                raise gdb.GdbError("CONFIG_STACKDEPOT is not set")

            argv = gdb.string_to_argv(args)
            if len(argv) != 1:
                _help()

            handle_int = int(argv[0], 16)
            _stack_depot_print(gdb.Value(handle_int).cast(utils.get_uint_type()))

    StackDepotLookup()
