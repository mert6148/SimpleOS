#!/usr/bin/env node
/**
 * SimpleOS script tools - küçük CLI yardımcıları.
 *
 * Amaç: script/ altındaki araçları tek giriş noktasından çalıştırmak.
 *
 * Örnek:
 *   node script/index.js env print
 *   node script/index.js run -- make qemu
 */

const { spawn } = require("node:child_process");
const fs = require("node:fs");
const path = require("node:path");

function printHelp(exitCode = 0) {
  const msg = `
SimpleOS script CLI

Kullanım:
  node script/index.js help
  node script/index.js env print
  node script/index.js run -- <komut> [args...]
  node script/index.js package list
  node script/index.js package install <package>
  node script/index.js php package/package.php list

Komutlar:
  help           Bu ekran
  env print      Proje kökündeki .env içeriğini güvenli biçimde yazdırır (yorumsuz)
  run            Komutu çapraz-platform çalıştırır (Windows/WSL/Linux)
  package        PHP paket yöneticisini çalıştırır
  php            Herhangi bir PHP betiğini çalıştırır
`;
  process.stdout.write(msg.trimStart());
  process.stdout.write("\n");
  process.exit(exitCode);
}

function findProjectRoot() {
  // Bu dosya: <root>/script/index.js
  return path.resolve(__dirname, "..");
}

function readDotEnvLines(dotEnvPath) {
  if (!fs.existsSync(dotEnvPath)) return [];
  const raw = fs.readFileSync(dotEnvPath, "utf8");
  return raw
    .split(/\r?\n/)
    .map((l) => l.trim())
    .filter((l) => l.length > 0 && !l.startsWith("#"));
}

async function runCommand(cmd, args, opts = {}) {
  return new Promise((resolve) => {
    const child = spawn(cmd, args, {
      stdio: "inherit",
      shell: false,
      ...opts,
    });
    child.on("close", (code) => resolve(code ?? 1));
    child.on("error", () => resolve(1));
  });
}

async function runPhpScript(scriptRelativePath, args) {
  const scriptPath = path.join(__dirname, scriptRelativePath);
  if (!fs.existsSync(scriptPath)) {
    process.stderr.write(`Hata: PHP betiği bulunamadı: ${scriptPath}\n`);
    process.exit(1);
  }

  const code = await runCommand("php", [scriptPath, ...args], {
    cwd: path.dirname(scriptPath),
  });
  process.exit(code);
}

async function main(argv) {
  const [sub, ...rest] = argv;
  const root = findProjectRoot();

  if (!sub || sub === "help" || sub === "--help" || sub === "-h") {
    printHelp(0);
  }

  if (sub === "env") {
    const action = rest[0];
    if (action !== "print") printHelp(1);

    const dotEnvPath = path.join(root, ".env");
    const lines = readDotEnvLines(dotEnvPath);
    for (const line of lines) process.stdout.write(line + "\n");
    return;
  }

  if (sub === "run") {
    const sep = rest.indexOf("--");
    const cmdParts = sep === -1 ? rest : rest.slice(sep + 1);
    if (cmdParts.length === 0) printHelp(1);

    const [cmd, ...args] = cmdParts;
    const code = await runCommand(cmd, args, { cwd: root });
    process.exit(code);
  }

  if (sub === "package") {
    const packageArgs = rest.length === 0 ? ["help"] : rest;
    await runPhpScript("package/package.php", packageArgs);
    return;
  }

  if (sub === "php") {
    if (rest.length === 0) {
      printHelp(1);
    }
    const [scriptName, ...phpArgs] = rest;
    await runPhpScript(scriptName, phpArgs);
    return;
  }

  printHelp(1);
}

main(process.argv.slice(2)).catch((err) => {
  process.stderr.write(String(err?.stack || err) + "\n");
  process.exit(1);
});