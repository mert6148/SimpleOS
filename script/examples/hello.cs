// SimpleOS script/examples - C# örneği
//
// Derleme (Windows, .NET SDK):
//   dotnet new console -n SimpleOSExample -o out
//   copy hello.cs out\Program.cs
//   dotnet run --project out

using System;

internal static class Program
{
    private static void Main()
    {
        Console.WriteLine("SimpleOS C# örnek: Merhaba Dünya");
        Console.WriteLine($"Zaman: {DateTimeOffset.Now:O}");
    }
}

