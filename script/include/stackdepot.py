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


def _parse_handle_argument(arg: str) -> Any:
    _require_gdb()
    if arg.lower().startswith("0x"):
        value = int(arg, 16)
    else:
        value = int(arg, 0)
    if value == 0:
        raise gdb.GdbError("handle is 0")
    return gdb.Value(value).cast(utils.get_uint_type())


def _stack_depot_fetch(handle: Any) -> Tuple[Any, int, int, int]:
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
    offset = int(parts["offset"]) << depot_stack_align

    pools_num = int(gdb.parse_and_eval("pools_num"))
    pool_index = int(parts["pool_index_plus_1"]) - 1

    if pool_index >= pools_num or pool_index < 0:
        raise gdb.GdbError(
            "pool index %d out of bounds (%d) for stack id %s"
            % (int(parts["pool_index"]), pools_num, handle)
        )

    stack_pools = gdb.parse_and_eval("stack_pools")
    pool = stack_pools[pool_index]
    stack = (
        pool + gdb.Value(offset).cast(utils.get_size_t_type())
    ).cast(stack_record_type.get_type().pointer())
    size = int(stack["size"].cast(utils.get_ulong_type()))
    return stack["entries"], size, pool_index, offset


def _stack_depot_print(handle: Any) -> None:
    _require_gdb()
    entries, nr_entries, pool_index, offset = _stack_depot_fetch(handle)
    gdb.write(
        "stack_depot handle=%s pool=%d offset=%d entries=%d\n"
        % (handle, pool_index, offset, nr_entries)
    )
    for i in range(nr_entries):
        try:
            address = int(entries[i])
            gdb.write("[%02d] 0x%x: " % (i, address))
            gdb.execute("x/i 0x%x" % address)
        except Exception as e:
            gdb.write("  <error> %s\n" % e)


def _stack_depot_info(handle: Any) -> None:
    _require_gdb()
    entries, nr_entries, pool_index, offset = _stack_depot_fetch(handle)
    gdb.write("Stack Depot Handle Information:\n")
    gdb.write("  handle: %s\n" % handle)
    gdb.write("  pool_index: %d\n" % pool_index)
    gdb.write("  offset: %d\n" % offset)
    gdb.write("  entry_count: %d\n" % nr_entries)
    gdb.write("  top_address: 0x%x\n" % int(entries[0]) if nr_entries > 0 else "  top_address: none\n")


def _stack_depot_status() -> None:
    _require_gdb()
    if not constants.LX_CONFIG_STACKDEPOT:
        raise gdb.GdbError("CONFIG_STACKDEPOT is not enabled")

    stack_depot_disabled = gdb.parse_and_eval("stack_depot_disabled")
    pools_num = gdb.parse_and_eval("pools_num")
    stack_pools = gdb.parse_and_eval("stack_pools")
    gdb.write("Stack Depot Status:\n")
    gdb.write("  CONFIG_STACKDEPOT: enabled\n")
    gdb.write("  stack_depot_disabled: %s\n" % stack_depot_disabled)
    gdb.write("  pools_num: %s\n" % pools_num)
    gdb.write("  stack_pools: %s\n" % stack_pools)


def _help_lookup() -> None:
    _require_gdb()
    text = """Usage: lx-stack_depot_lookup [handle]
Print the stack record entries for the given handle.
Example:
  lx-stack_depot_lookup 0x00c80300
"""
    raise gdb.GdbError(text)


def _help_info() -> None:
    _require_gdb()
    text = """Usage: lx-stack_depot_info [handle]
Print metadata for the given stack depot handle.
Example:
  lx-stack_depot_info 0x00c80300
"""
    raise gdb.GdbError(text)


def _help_status() -> None:
    _require_gdb()
    text = """Usage: lx-stack_depot_status
Print current stack depot runtime status.
"""
    raise gdb.GdbError(text)


if gdb is not None:

    class StackDepotLookup(gdb.Command):
        """Search stack entry backtrace by handle"""

        def __init__(self) -> None:
            super().__init__("lx-stack_depot_lookup", gdb.COMMAND_SUPPORT)

        def invoke(self, args: str, from_tty: bool) -> None:
            if not constants.LX_CONFIG_STACKDEPOT:
                raise gdb.GdbError("CONFIG_STACKDEPOT is not set")
            argv = gdb.string_to_argv(args)
            if len(argv) != 1:
                _help_lookup()
            handle = _parse_handle_argument(argv[0])
            _stack_depot_print(handle)


    class StackDepotInfo(gdb.Command):
        """Display metadata for a stack depot handle"""

        def __init__(self) -> None:
            super().__init__("lx-stack_depot_info", gdb.COMMAND_SUPPORT)

        def invoke(self, args: str, from_tty: bool) -> None:
            if not constants.LX_CONFIG_STACKDEPOT:
                raise gdb.GdbError("CONFIG_STACKDEPOT is not set")
            argv = gdb.string_to_argv(args)
            if len(argv) != 1:
                _help_info()
            handle = _parse_handle_argument(argv[0])
            _stack_depot_info(handle)


    class StackDepotStatus(gdb.Command):
        """Show stack depot subsystem status"""

        def __init__(self) -> None:
            super().__init__("lx-stack_depot_status", gdb.COMMAND_SUPPORT)

        def invoke(self, args: str, from_tty: bool) -> None:
            if args.strip():
                _help_status()
            _stack_depot_status()


    StackDepotLookup()
    StackDepotInfo()
    StackDepotStatus()
