using System;
using System.IO;
using System.Diagnostics;
using System.Linq;

namespace SimpleOS.SystemProgram
{
    internal static class Program
    {
        static int Main(string[] args)
        {
            var showHelp = args.Length == 0 || args.Contains("--help") || args.Contains("-h");
            if (showHelp)
            {
                PrintHelp();
                return 0;
            }

            if (args.Contains("--arch"))
            {
                Console.WriteLine(GetArchitectureInfo());
            }

            if (args.Contains("--list-tables"))
            {
                var tableDir = Path.Combine("..", "@table");
                if (!Directory.Exists(tableDir)) tableDir = Path.Combine("include", "@table");
                if (!Directory.Exists(tableDir)) tableDir = Path.Combine(Directory.GetCurrentDirectory(), "include", "@table");
                if (Directory.Exists(tableDir))
                {
                    foreach (var f in Directory.EnumerateFiles(tableDir, "*.csv")) Console.WriteLine(Path.GetFileName(f));
                }
                else Console.WriteLine("Table directory not found: include/@table");
            }

            var printIndex = Array.IndexOf(args, "--print");
            if (printIndex >= 0 && printIndex + 1 < args.Length)
            {
                var file = args[printIndex + 1];
                if (!File.Exists(file))
                {
                    Console.Error.WriteLine($"File not found: {file}");
                    return 2;
                }
                var contents = File.ReadAllText(file);
                Console.WriteLine(contents);
            }

            var runCLI = args.Contains("--run-table-cli");
            if (runCLI)
            {
                // Try to run table_cli in include/@table
                var cliPath = Path.Combine("include", "@table", "table_cli.exe");
                if (!File.Exists(cliPath)) cliPath = Path.Combine("include", "@table", "table_cli");
                if (File.Exists(cliPath))
                {
                    var psi = new ProcessStartInfo { FileName = cliPath, UseShellExecute = false };
                    psi.RedirectStandardInput = true; psi.RedirectStandardOutput = true; psi.RedirectStandardError = true;
                    using var p = Process.Start(psi)!;
                    p.OutputDataReceived += (s,e)=> { if (e.Data!=null) Console.WriteLine(e.Data); };
                    p.BeginOutputReadLine();
                    p.BeginErrorReadLine();
                    p.WaitForExit();
                }
                else Console.WriteLine("table_cli not found in include/@table");
            }

            return 0;
        }

        static void PrintHelp()
        {
            Console.WriteLine("SimpleOS .NET System Program");
            Console.WriteLine("Usage:");
            Console.WriteLine("  --arch                Show architecture information");
            Console.WriteLine("  --list-tables         List CSV tables in include/@table");
            Console.WriteLine("  --print <file.csv>    Print a CSV file to console");
            Console.WriteLine("  --run-table-cli       Run include/@table/table_cli if available");
        }

        static string GetArchitectureInfo()
        {
            var osBits = Environment.Is64BitOperatingSystem ? 64 : 32;
            var proc = Environment.GetEnvironmentVariable("PROCESSOR_IDENTIFIER") ?? Environment.MachineName;
            return $"Architecture: {(osBits==64?"x64":"x86")}, Bits: {osBits}, Processor: {proc}, .NET: {Environment.Version}";
        }
    }
}
