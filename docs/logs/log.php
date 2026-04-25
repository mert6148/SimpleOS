<?php
/**
 * SimpleOS Documentation Log
 * This file serves as a log for recent edits and changes made to the SimpleOS documentation. It helps track modifications and updates to ensure the documentation remains accurate and up-to-date.
 * @version 1.0.0
 * @author Mert Doğanay
 * @license MIT
 * @see {@link https://github.com/mertdoganay/SimpleOS/blob/main/docs/logs/log.php} for the source code of this log file.
 * Note: This log is intended for internal use by the documentation team and may not be accessible to the public. It is recommended to keep this file secure and only share it with authorized personnel.
 * Change Log:
 * - 2026-04-21: Initial creation of the log file.
 * - 2026-04-21: Added recent edits from the documentation, including changes to the footer and author information.
 * - 2026-04-21: Updated the log format and added additional metadata for better tracking of changes.
 * For any questions or concerns regarding the documentation or this log, please contact the documentation team at
 */

header("Content-Type: text/plain");
header("Cache-Control: no-cache, no-store, must-revalidate");
header("Pragma: no-cache");
header("Expires: 0");

// =============================================================
// SimpleOS Documentation Log
// =============================================================
// This log tracks recent edits and changes made to the SimpleOS documentation.

header("Content-Type: text/plain");
header("Cache-Control: no-cache, no-store, must-revalidate");
header("Pragma: no-cache");<?php
$sayi = 100;

// 1. Doğal Logaritma (ln - e tabanında)
$ln_sonuc = log($sayi); 

// 2. 10'luk Tabanda Logaritma
$log10_sonuc = log10($sayi); 

// 3. Özel Tabanda Logaritma (Örn: 2'lik taban)
$taban = 2;
$log2_sonuc = log($sayi, $taban);

// Sonuçları Yazdıralım
echo "Sayı: $sayi <br>";
echo "Doğal Logaritma (ln): " . round($ln_sonuc, 4) . "<br>";
echo "10'luk Tabanda Log: " . $log10_sonuc . "<br>";
echo "2'lik Tabanda Log: " . round($log2_sonuc, 4);
?>
