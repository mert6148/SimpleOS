# SimpleOS Linux Distribution

Bu klasör, SimpleOS için x86 ve x64 Linux sistem derlemeleri ile dağıtım paketleme araçlarını içerir.

## Adımlar

1. x86 sistemi derleyin:

```bash
cd linux/x86
make
```

2. x64 sistemi derleyin:

```bash
cd linux/x64
make
```

3. Dağıtımı paketleyin:

```bash
dotnet run --project linux/distribution/SimpleOSLinuxDist.csproj -- --root . --package
```

4. Hem inşa et hem paketle:

```bash
dotnet run --project linux/distribution/SimpleOSLinuxDist.csproj -- --root . --build --package
```

## Sonuç

- `linux/dist/linux-x86/`
- `linux/dist/linux-x64/`
- `linux/simpleos-linux-distribution.zip`
