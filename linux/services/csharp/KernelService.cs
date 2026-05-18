using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;

namespace SimpleOS.Services
{
    /// <summary>
    /// Kernel service for interacting with SimpleOS kernel
    /// </summary>
    [ApiController]
    [Route("api/[controller]")]
    public class KernelController : ControllerBase
    {
        private readonly IKernelService _kernelService;
        private readonly ILogger<KernelController> _logger;

        public KernelController(IKernelService kernelService, ILogger<KernelController> logger)
        {
            _kernelService = kernelService;
            _logger = logger;
        }

        /// <summary>
        /// Get list of all running processes
        /// </summary>
        [HttpGet("processes")]
        public async Task<ActionResult<ApiResponse<List<ProcessInfo>>>> GetProcesses()
        {
            try
            {
                var processes = await _kernelService.GetProcessesAsync();
                return Ok(new ApiResponse<List<ProcessInfo>>
                {
                    Status = "success",
                    Data = processes,
                    Timestamp = DateTime.UtcNow
                });
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting processes");
                return StatusCode(500, new ApiResponse<List<ProcessInfo>>
                {
                    Status = "error",
                    Message = ex.Message,
                    Timestamp = DateTime.UtcNow
                });
            }
        }

        /// <summary>
        /// Get system statistics
        /// </summary>
        [HttpGet("stats")]
        public async Task<ActionResult<ApiResponse<SystemStats>>> GetSystemStats()
        {
            try
            {
                var stats = await _kernelService.GetSystemStatsAsync();
                return Ok(new ApiResponse<SystemStats>
                {
                    Status = "success",
                    Data = stats,
                    Timestamp = DateTime.UtcNow
                });
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting system stats");
                return StatusCode(500, new ApiResponse<SystemStats>
                {
                    Status = "error",
                    Message = ex.Message,
                    Timestamp = DateTime.UtcNow
                });
            }
        }

        /// <summary>
        /// Get detailed process information
        /// </summary>
        [HttpGet("process/{pid}")]
        public async Task<ActionResult<ApiResponse<ProcessInfo>>> GetProcessInfo(int pid)
        {
            try
            {
                var processInfo = await _kernelService.GetProcessInfoAsync(pid);
                if (processInfo == null)
                {
                    return NotFound(new ApiResponse<ProcessInfo>
                    {
                        Status = "error",
                        Message = $"Process {pid} not found",
                        Timestamp = DateTime.UtcNow
                    });
                }

                return Ok(new ApiResponse<ProcessInfo>
                {
                    Status = "success",
                    Data = processInfo,
                    Timestamp = DateTime.UtcNow
                });
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting process info");
                return StatusCode(500, new ApiResponse<ProcessInfo>
                {
                    Status = "error",
                    Message = ex.Message,
                    Timestamp = DateTime.UtcNow
                });
            }
        }

        /// <summary>
        /// Kill a process
        /// </summary>
        [HttpPost("process/{pid}/kill")]
        public async Task<ActionResult<ApiResponse<bool>>> KillProcess(int pid)
        {
            try
            {
                var result = await _kernelService.KillProcessAsync(pid);
                return Ok(new ApiResponse<bool>
                {
                    Status = result ? "success" : "error",
                    Data = result,
                    Message = result ? "Process killed successfully" : "Failed to kill process",
                    Timestamp = DateTime.UtcNow
                });
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error killing process");
                return StatusCode(500, new ApiResponse<bool>
                {
                    Status = "error",
                    Message = ex.Message,
                    Timestamp = DateTime.UtcNow
                });
            }
        }
    }

    /// <summary>
    /// Kernel service interface
    /// </summary>
    public interface IKernelService
    {
        Task<List<ProcessInfo>> GetProcessesAsync();
        Task<SystemStats> GetSystemStatsAsync();
        Task<ProcessInfo> GetProcessInfoAsync(int pid);
        Task<bool> KillProcessAsync(int pid);
    }

    /// <summary>
    /// Implementation of kernel service
    /// </summary>
    public class KernelService : IKernelService
    {
        private readonly ILogger<KernelService> _logger;

        public KernelService(ILogger<KernelService> logger)
        {
            _logger = logger;
        }

        public async Task<List<ProcessInfo>> GetProcessesAsync()
        {
            return await Task.Run(() =>
            {
                var processes = new List<ProcessInfo>();

                // Get processes from system
                var processList = Process.GetProcesses();
                foreach (var process in processList)
                {
                    try
                    {
                        processes.Add(new ProcessInfo
                        {
                            Pid = process.Id,
                            Name = process.ProcessName,
                            Memory = process.WorkingSet64 / 1024, // KB
                            Priority = (int)process.PriorityClass,
                            State = process.Responding ? "running" : "not responding"
                        });
                    }
                    catch
                    {
                        // Skip inaccessible processes
                    }
                }

                return processes.OrderByDescending(p => p.Memory).ToList();
            });
        }

        public async Task<SystemStats> GetSystemStatsAsync()
        {
            return await Task.Run(() =>
            {
                var totalMemory = GC.GetTotalMemory(false);
                var cpuUsage = GetCpuUsage();

                return new SystemStats
                {
                    TotalMemory = totalMemory / (1024 * 1024), // MB
                    UsedMemory = totalMemory / (1024 * 1024),
                    FreeMemory = (GC.GetTotalMemory(true) / (1024 * 1024)),
                    CpuUsage = cpuUsage,
                    LoadAverage = GetLoadAverage(),
                    Timestamp = DateTime.UtcNow
                };
            });
        }

        public async Task<ProcessInfo> GetProcessInfoAsync(int pid)
        {
            return await Task.Run(() =>
            {
                try
                {
                    var process = Process.GetProcessById(pid);
                    return new ProcessInfo
                    {
                        Pid = process.Id,
                        Name = process.ProcessName,
                        Memory = process.WorkingSet64 / 1024,
                        Priority = (int)process.PriorityClass,
                        State = process.Responding ? "running" : "not responding",
                        StartTime = process.StartTime,
                        CpuTime = (long)process.TotalProcessorTime.TotalMilliseconds
                    };
                }
                catch
                {
                    return null;
                }
            });
        }

        public async Task<bool> KillProcessAsync(int pid)
        {
            return await Task.Run(() =>
            {
                try
                {
                    var process = Process.GetProcessById(pid);
                    process.Kill();
                    return true;
                }
                catch
                {
                    return false;
                }
            });
        }

        private float GetCpuUsage()
        {
            var cpuCounter = new PerformanceCounter("Processor", "% Processor Time", "_Total", true);
            cpuCounter.NextValue();
            System.Threading.Thread.Sleep(100);
            return cpuCounter.NextValue();
        }

        private float GetLoadAverage()
        {
            var cpuCounter = new PerformanceCounter("Processor", "% Processor Time", "_Total", true);
            return cpuCounter.NextValue();
        }
    }

    /// <summary>
    /// Data models
    /// </summary>
    public class ProcessInfo
    {
        public int Pid { get; set; }
        public string Name { get; set; }
        public long Memory { get; set; }
        public int Priority { get; set; }
        public string State { get; set; }
        public DateTime StartTime { get; set; }
        public long CpuTime { get; set; }
    }

    public class SystemStats
    {
        public long TotalMemory { get; set; }
        public long UsedMemory { get; set; }
        public long FreeMemory { get; set; }
        public float CpuUsage { get; set; }
        public float LoadAverage { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class ApiResponse<T>
    {
        public string Status { get; set; }
        public T Data { get; set; }
        public string Message { get; set; }
        public DateTime Timestamp { get; set; }
    }
}