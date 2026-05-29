using System;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Diagnostics;

namespace SimpleOS.LinuxDist
{
    internal static class Program
    {
        private static readonly string[] BinaryRules = { "x86_system", "x64_system" };

        static int Main(string[] args)
        {
            var root = GetRootDirectory(args);
            var build = args.Contains("--build");
            var package = args.Contains("--package") || !args.Any();

            if (args.Contains("--help") || args.Contains("-h"))
            {
                PrintHelp();
                return 0;
            }

            if (build)
            {
                if (!BuildArchitectures(root)) return 1;
            }

            if (package)
            {
                PackageDistribution(root);
            }

            return 0;
        }

        private static string GetRootDirectory(string[] args)
        {
            var rootArgIndex = Array.FindIndex(args, a => a == "--root");
            if (rootArgIndex >= 0 && rootArgIndex + 1 < args.Length)
            {
                return Path.GetFullPath(args[rootArgIndex + 1]);
            }

            return Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, "..", "..", ".."));
        }

        private static bool BuildArchitectures(string root)
        {
            Console.WriteLine($"Building architectures in {root}...");
            return RunCommand("make", new[] { "all" }, Path.Combine(root, "linux", "x86"))
                && RunCommand("make", new[] { "all" }, Path.Combine(root, "linux", "x64"));
        }

        private static bool PackageDistribution(string root)
        {
            var distDir = Path.Combine(root, "linux", "dist");
            Directory.CreateDirectory(distDir);

            var x86Source = Path.Combine(root, "linux", "x86", "x86_system");
            var x64Source = Path.Combine(root, "linux", "x64", "x64_system");

            if (!File.Exists(x86Source) || !File.Exists(x64Source))
            {
                Console.Error.WriteLine("Build artifacts not found. Run with --build first.");
                return false;
            }

            CopyPackage(x86Source, Path.Combine(distDir, "linux-x86"));
            CopyPackage(x64Source, Path.Combine(distDir, "linux-x64"));

            CreateManifest(distDir);
            CreateZip(distDir, Path.Combine(root, "linux", "simpleos-linux-distribution.zip"));

            Console.WriteLine($"Linux distribution package created in {distDir}");
            return true;
        }

        private static void CopyPackage(string binaryPath, string targetFolder)
        {
            Directory.CreateDirectory(targetFolder);
            var fileName = Path.GetFileName(binaryPath);
            File.Copy(binaryPath, Path.Combine(targetFolder, fileName), overwrite: true);
            File.WriteAllText(Path.Combine(targetFolder, "README.txt"), $"SimpleOS Linux package for {fileName}\n");
        }

        private static void CreateManifest(string distDir)
        {
            var manifestPath = Path.Combine(distDir, "manifest.txt");
            var lines = BinaryRules.Select(rule => rule switch
            {
                "x86_system" => "linux-x86: 32-bit x86 SimpleOS system binary",
                "x64_system" => "linux-x64: 64-bit x64 SimpleOS system binary",
                _ => rule
            }).ToArray();
            File.WriteAllLines(manifestPath, lines);
        }

        private static void CreateZip(string sourceDir, string zipPath)
        {
            if (File.Exists(zipPath)) File.Delete(zipPath);
            ZipFile.CreateFromDirectory(sourceDir, zipPath, CompressionLevel.Optimal, includeBaseDirectory: false);
            Console.WriteLine($"Created distribution archive: {zipPath}");
        }

        private static bool RunCommand(string command, string[] arguments, string workingDirectory)
        {
            Console.WriteLine($"Running {command} in {workingDirectory}");
            var psi = new ProcessStartInfo { FileName = command, Arguments = string.Join(' ', arguments), WorkingDirectory = workingDirectory, RedirectStandardOutput = true, RedirectStandardError = true, UseShellExecute = false };
            using var process = Process.Start(psi)!;
            process.OutputDataReceived += (s, e) => { if (e.Data is not null) Console.WriteLine(e.Data); };
            process.ErrorDataReceived += (s, e) => { if (e.Data is not null) Console.Error.WriteLine(e.Data); };
            process.BeginOutputReadLine();
            process.BeginErrorReadLine();
            process.WaitForExit();
            return process.ExitCode == 0;
        }

        private static void PrintHelp()
        {
            Console.WriteLine("SimpleOS Linux Distribution Builder");
            Console.WriteLine("Usage: dotnet run --project linux/distribution/SimpleOSLinuxDist.csproj [--build] [--package] [--root <path>] [--help]");
            Console.WriteLine("  --build    Build x86 and x64 system binaries.");
            Console.WriteLine("  --package  Package built binaries into linux/dist and create a zip archive.");
            Console.WriteLine("  --root     Set the repository root directory.");
            Console.WriteLine("  --help     Show this help text.");
        }
    }
}
