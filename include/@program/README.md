# SimpleSystem (.NET) - SimpleOS system program

This folder contains a small .NET console application that provides system utilities for the SimpleOS project.

Prerequisites
- .NET SDK (6.0/7.0+) installed. Verify with `dotnet --version`.

Build

```bash
cd include/@program
dotnet build -c Release
```

Run

```bash
# show architecture
dotnet run --project include/@program -- --arch

# list CSV tables
dotnet run --project include/@program -- --list-tables

# print a CSV file
dotnet run --project include/@program -- --print ../@table/sample_users.csv

# attempt to run the native table CLI if built
dotnet run --project include/@program -- --run-table-cli
```

Notes
- The program is intentionally small and demonstrates interop with project files (CSV tables) and launching external tools when present.
