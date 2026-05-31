# Factory (C) - SimpleOS include/@factory

This folder contains a small C implementation of a Factory pattern and a CLI demo.

Protection:
- The CLI requires an environment variable `FACTORY_SECRET` to be set.
- You must pass `--key <secret>` and it must match `FACTORY_SECRET`.

Build and run (from this folder):

```sh
make
# set secret and run
export FACTORY_SECRET=mysecret   # on Windows PowerShell: $Env:FACTORY_SECRET='mysecret'
./factory_app --key mysecret Gadget Widget
```

Files:
- `Factory.h` - header with API and include guard
- `Factory.c` - implementation
- `main.c` - CLI demo with protection
- `Makefile` - build helper
