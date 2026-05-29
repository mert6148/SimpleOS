# SimpleOS Linux Distribution Builder

Bu proje, SimpleOS için x86 ve x64 Linux sistem derlemelerini paketleyen bir C# aracı içerir.

## Derleme

```bash
cd linux/distribution
dotnet build
```

## Çalıştırma

```bash
cd linux/distribution
dotnet run --project SimpleOSLinuxDist.csproj -- --build --package
```

## Argümanlar

- `--build` : `linux/x86` ve `linux/x64` dizinlerinde `make all` çalıştırır.
- `--package` : `linux/dist` dizinine paketler ve `linux/simpleos-linux-distribution.zip` arşivini oluşturur.
- `--root <path>` : repository kök dizinini belirtir.
- `--help` : yardım metnini gösterir.
