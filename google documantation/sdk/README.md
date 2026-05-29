# Google Documentation SDK

This folder contains a simple C# SDK project for documentation workflows.

## Project structure

- `GoogleDocsSDK.csproj` - .NET project file
- `documentation.cs` - document model and Markdown-to-HTML rendering
- `googledocs.cs` - document service and client wrapper
- `sys/Program.cs` - console sample demonstrating usage

## Build and run

```powershell
cd "google documantation/sdk"
dotnet build
dotnet run --project GoogleDocsSDK.csproj
```

## Features

- Create and manage documentation objects
- Search documents by title or content
- Export a document to simple HTML
- Sample console app showing SDK usage
