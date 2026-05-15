/**
 * =============================================================================
 * logs.js — Kapsamlı Logaritma Kütüphanesi
 * =============================================================================
 * Açıklama : Matematiksel logaritma işlemleri, seri açılımları, nümerik
 *            yöntemler, istatistiksel fonksiyonlar ve yardımcı araçlar.
 * Sürüm    : 3.0.0
 * Lisans   : MIT
 * =============================================================================
 */

"use strict";

// =============================================================================
// BÖLÜM 1 — SABİTLER VE YAPILANDIRMA
// =============================================================================

/** Doğal logaritma tabanı */
const E = Math.E;

/** Pi sayısı */
const PI = Math.PI;

/** Makine epsilon değeri (double precision) */
const EPSILON = Number.EPSILON;

/** Maksimum iterasyon sayısı (seri hesaplamalar için) */
const MAX_ITER = 1000;

/** Yakınsama toleransı */
const TOL = 1e-15;

/** log(2) sabit değeri */
const LN2 = Math.LN2;

/** log(10) sabit değeri */
const LN10 = Math.LN10;

/** log2(e) sabit değeri */
const LOG2E = Math.LOG2E;

/** log10(e) sabit değeri */
const LOG10E = Math.LOG10E;

/** Euler-Mascheroni sabiti γ ≈ 0.5772... */
const GAMMA_EM = 0.5772156649015328606065120900824024310421593359;

/** Altın oran φ */
const PHI = (1 + Math.sqrt(5)) / 2;

/** ln(φ) */
const LN_PHI = Math.log(PHI);

/** ln(2π) */
const LN_2PI = Math.log(2 * PI);

/** Maksimum güvenli üs (overflow sınırı) */
const MAX_EXP = 709.782712893384;

/** Minimum güvenli üs (underflow sınırı) */
const MIN_EXP = -708.396418532264;

// =============================================================================
// BÖLÜM 2 — TEMEL LOGARİTMA FONKSİYONLARI
// =============================================================================

/**
 * Doğal logaritma — ln(x)
 * @param {number} x — pozitif gerçek sayı
 * @returns {number} ln(x)
 * @throws {RangeError} x <= 0 ise
 */
function ln(x) {
    if (typeof x !== "number" || isNaN(x)) {
        throw new TypeError("ln: Girdi sayısal olmalıdır.");
    }
    if (x <= 0) {
        throw new RangeError(`ln: Tanım kümesi x > 0 iken x = ${x} verildi.`);
    }
    if (x === Infinity) return Infinity;
    return Math.log(x);
}

/**
 * 10 tabanında logaritma — log₁₀(x)
 * @param {number} x — pozitif gerçek sayı
 * @returns {number} log₁₀(x)
 */
function log10(x) {
    if (x <= 0) throw new RangeError(`log10: x > 0 olmalıdır, x = ${x}`);
    return Math.log10(x);
}

/**
 * 2 tabanında logaritma — log₂(x)
 * @param {number} x — pozitif gerçek sayı
 * @returns {number} log₂(x)
 */
function log2(x) {
    if (x <= 0) throw new RangeError(`log2: x > 0 olmalıdır, x = ${x}`);
    return Math.log2(x);
}

/**
 * Keyfi tabanda logaritma — log_b(x)
 * @param {number} x    — pozitif gerçek sayı
 * @param {number} taban — pozitif, 1'den farklı taban
 * @returns {number} log_b(x)
 */
function logTaban(x, taban) {
    if (x <= 0) throw new RangeError(`logTaban: x > 0 olmalıdır.`);
    if (taban <= 0) throw new RangeError(`logTaban: taban > 0 olmalıdır.`);
    if (taban === 1) throw new RangeError(`logTaban: taban ≠ 1 olmalıdır.`);
    return Math.log(x) / Math.log(taban);
}

/**
 * 1 + x için doğal logaritma — ln(1 + x)
 * Küçük x değerleri için sayısal olarak daha kararlı hesaplama.
 * @param {number} x — x > -1
 * @returns {number} ln(1 + x)
 */
function log1p(x) {
    if (x <= -1) throw new RangeError(`log1p: x > -1 olmalıdır, x = ${x}`);
    return Math.log1p(x);
}

/**
 * Negatif logaritma — -ln(x)
 * Bilgi teorisi ve olasılık hesaplarında kullanılır.
 * @param {number} x — pozitif gerçek sayı
 * @returns {number} -ln(x)
 */
function negatifLog(x) {
    return -ln(x);
}

/**
 * Mutlak değerin logaritması — ln|x|
 * @param {number} x — sıfırdan farklı gerçek sayı
 * @returns {number} ln|x|
 */
function logMutlak(x) {
    if (x === 0) throw new RangeError("logMutlak: x ≠ 0 olmalıdır.");
    return Math.log(Math.abs(x));
}

/**
 * Logaritmanın işaret fonksiyonu ile birleşimi — sign(x) * ln|x|
 * @param {number} x — sıfırdan farklı gerçek sayı
 * @returns {number}
 */
function isareliLog(x) {
    if (x === 0) return 0;
    return Math.sign(x) * logMutlak(x);
}

/**
 * Tavan logaritması — ⌈log_b(x)⌉
 * @param {number} x    — pozitif gerçek sayı
 * @param {number} taban — taban
 * @returns {number}
 */
function tavanLog(x, taban = 2) {
    return Math.ceil(logTaban(x, taban));
}

/**
 * Taban logaritması — ⌊log_b(x)⌋
 * @param {number} x    — pozitif gerçek sayı
 * @param {number} taban — taban
 * @returns {number}
 */
function tabanLog(x, taban = 2) {
    return Math.floor(logTaban(x, taban));
}

// =============================================================================
// BÖLÜM 3 — LOGARİTMANIN TEMEL ÖZELLİKLERİ VE KİMLİKLERİ
// =============================================================================

/**
 * Çarpım kuralı: log_b(x * y) = log_b(x) + log_b(y)
 * @param {number} x
 * @param {number} y
 * @param {number} taban
 * @returns {number}
 */
function carpimKurali(x, y, taban = E) {
    if (x <= 0 || y <= 0) throw new RangeError("carpimKurali: x, y > 0 olmalıdır.");
    return logTaban(x, taban) + logTaban(y, taban);
}

/**
 * Bölme kuralı: log_b(x / y) = log_b(x) - log_b(y)
 * @param {number} x
 * @param {number} y
 * @param {number} taban
 * @returns {number}
 */
function bolmeKurali(x, y, taban = E) {
    if (x <= 0 || y <= 0) throw new RangeError("bolmeKurali: x, y > 0 olmalıdır.");
    return logTaban(x, taban) - logTaban(y, taban);
}

/**
 * Üs kuralı: log_b(x^n) = n * log_b(x)
 * @param {number} x
 * @param {number} n
 * @param {number} taban
 * @returns {number}
 */
function usKurali(x, n, taban = E) {
    if (x <= 0) throw new RangeError("usKurali: x > 0 olmalıdır.");
    return n * logTaban(x, taban);
}

/**
 * Taban değişim formülü: log_b(x) = log_a(x) / log_a(b)
 * @param {number} x       — sayı
 * @param {number} yeniTaban — hedef taban
 * @param {number} eskiTaban — kaynak taban
 * @returns {number}
 */
function tabanDegisimi(x, yeniTaban, eskiTaban = E) {
    return logTaban(x, eskiTaban) / logTaban(yeniTaban, eskiTaban);
}

/**
 * Tersine logaritma — exp(y) = x ise y = ln(x) ifadesinden x hesapla
 * @param {number} y
 * @returns {number} e^y
 */
function tersiLog(y) {
    return Math.exp(y);
}

/**
 * Logaritmik ortalama: L(a, b) = (b - a) / (ln(b) - ln(a))
 * @param {number} a
 * @param {number} b
 * @returns {number}
 */
function logaritmikOrtalama(a, b) {
    if (a <= 0 || b <= 0) throw new RangeError("logaritmikOrtalama: a, b > 0 olmalıdır.");
    if (a === b) return a;
    return (b - a) / (Math.log(b) - Math.log(a));
}

/**
 * Geometrik ortalama: GM(a, b) = exp((ln(a) + ln(b)) / 2)
 * @param {number} a
 * @param {number} b
 * @returns {number}
 */
function geometrikOrtalama(a, b) {
    if (a <= 0 || b <= 0) throw new RangeError("geometrikOrtalama: a, b > 0 olmalıdır.");
    return Math.exp((Math.log(a) + Math.log(b)) / 2);
}

/**
 * n elemanlı dizinin geometrik ortalaması
 * @param {number[]} dizi
 * @returns {number}
 */
function geometrikOrtalamaArray(dizi) {
    if (!dizi || dizi.length === 0) throw new Error("geometrikOrtalamaArray: Boş dizi.");
    const toplamLog = dizi.reduce((acc, x) => {
        if (x <= 0) throw new RangeError("geometrikOrtalamaArray: Tüm elemanlar > 0 olmalıdır.");
        return acc + Math.log(x);
    }, 0);
    return Math.exp(toplamLog / dizi.length);
}

// =============================================================================
// BÖLÜM 4 — SERİ AÇILIMI İLE LOGARİTMA HESABI
// =============================================================================

/**
 * Taylor serisi ile ln(1 + x) hesabı — |x| < 1 için
 * ln(1+x) = x - x²/2 + x³/3 - x⁴/4 + ...
 * @param {number} x — |x| < 1
 * @param {number} terimSayisi — kullanılacak terim sayısı
 * @returns {number}
 */
function taylorLog1p(x, terimSayisi = 100) {
    if (Math.abs(x) >= 1) {
        throw new RangeError("taylorLog1p: |x| < 1 koşulu gereklidir.");
    }
    let sonuc = 0;
    let kuvvet = x;
    for (let n = 1; n <= terimSayisi; n++) {
        sonuc += (n % 2 === 0 ? -1 : 1) * kuvvet / n;
        kuvvet *= x;
        if (Math.abs(kuvvet / n) < TOL) break;
    }
    return sonuc;
}

/**
 * Mercator serisi ile ln(x) hesabı
 * x = (1+t)/(1-t) dönüşümü ile |t| < 1 sağlanır.
 * ln(x) = 2 * (t + t³/3 + t⁵/5 + ...)
 * @param {number} x — x > 0
 * @param {number} terimSayisi — terim sayısı
 * @returns {number}
 */
function mercatorLog(x, terimSayisi = 200) {
    if (x <= 0) throw new RangeError("mercatorLog: x > 0 olmalıdır.");
    const t = (x - 1) / (x + 1);
    let sonuc = 0;
    let kuvvet = t;
    const tKare = t * t;
    for (let n = 0; n < terimSayisi; n++) {
        const payda = 2 * n + 1;
        sonuc += kuvvet / payda;
        kuvvet *= tKare;
        if (Math.abs(kuvvet / payda) < TOL) break;
    }
    return 2 * sonuc;
}

/**
 * AGM (Aritmetik-Geometrik Ortalama) tabanlı ln hesabı
 * Yüksek hassasiyetli hesaplamalar için Gauss AGM yöntemi.
 * @param {number} x — x > 0
 * @returns {number}
 */
function agmLog(x) {
    if (x <= 0) throw new RangeError("agmLog: x > 0 olmalıdır.");
    // ln(x) ≈ π / (2 * AGM(1, 4/x)) — büyük x için
    // Küçük x için düzeltme
    const n = Math.ceil(Math.log2(x));
    const y = x / Math.pow(2, n);
    let a = 1;
    let g = 4 / y;
    for (let i = 0; i < 50; i++) {
        const aYeni = (a + g) / 2;
        const gYeni = Math.sqrt(a * g);
        if (Math.abs(aYeni - gYeni) < TOL) {
            return PI / (2 * aYeni) + n * LN2;
        }
        a = aYeni;
        g = gYeni;
    }
    return PI / (2 * a) + n * LN2;
}

/**
 * Newton-Raphson yöntemi ile ln(x) hesabı
 * f(y) = e^y - x = 0 denklemini iteratif çözer.
 * @param {number} x — x > 0
 * @param {number} maxIter — maksimum iterasyon
 * @returns {number}
 */
function newtonLog(x, maxIter = 50) {
    if (x <= 0) throw new RangeError("newtonLog: x > 0 olmalıdır.");
    // Başlangıç tahmini
    let y = (x - 1) / (x + 1) * 2;
    for (let i = 0; i < maxIter; i++) {
        const ey = Math.exp(y);
        const yYeni = y - (ey - x) / ey;
        if (Math.abs(yYeni - y) < TOL) return yYeni;
        y = yYeni;
    }
    return y;
}

/**
 * Halley yöntemi ile ln(x) hesabı (kübik yakınsama)
 * @param {number} x — x > 0
 * @param {number} maxIter — maksimum iterasyon
 * @returns {number}
 */
function halleyLog(x, maxIter = 20) {
    if (x <= 0) throw new RangeError("halleyLog: x > 0 olmalıdır.");
    let y = (x - 1) / (x + 1) * 2;
    for (let i = 0; i < maxIter; i++) {
        const ey = Math.exp(y);
        const frac = (ey - x) / (ey + x);
        const yYeni = y - 2 * frac;
        if (Math.abs(yYeni - y) < TOL) return yYeni;
        y = yYeni;
    }
    return y;
}

// =============================================================================
// BÖLÜM 5 — BİLGİ TEORİSİ
// =============================================================================

/**
 * Binary entropy — H₂(p) = -p*log₂(p) - (1-p)*log₂(1-p)
 * @param {number} p — olasılık [0, 1]
 * @returns {number} bit cinsinden entropi
 */
function ikililiEntropi(p) {
    if (p < 0 || p > 1) throw new RangeError("ikililiEntropi: 0 ≤ p ≤ 1 olmalıdır.");
    if (p === 0 || p === 1) return 0;
    return -(p * Math.log2(p) + (1 - p) * Math.log2(1 - p));
}

/**
 * Shannon entropisi — H(X) = -Σ p(x) * log₂(p(x))
 * @param {number[]} olasiliklar — olasılık dağılımı
 * @returns {number} bit cinsinden entropi
 */
function shannonEntropi(olasiliklar) {
    const toplam = olasiliklar.reduce((a, b) => a + b, 0);
    if (Math.abs(toplam - 1) > 1e-9) {
        throw new Error(`shannonEntropi: Olasılıklar toplamı 1 olmalı, ${toplam} bulundu.`);
    }
    return -olasiliklar.reduce((acc, p) => {
        if (p < 0) throw new RangeError("shannonEntropi: Olasılıklar negatif olamaz.");
        if (p === 0) return acc;
        return acc + p * Math.log2(p);
    }, 0);
}

/**
 * Çapraz entropi — H(p, q) = -Σ p(x) * log(q(x))
 * @param {number[]} p — gerçek dağılım
 * @param {number[]} q — tahmin dağılımı
 * @returns {number}
 */
function caprazEntropi(p, q) {
    if (p.length !== q.length) throw new Error("caprazEntropi: Dizi uzunlukları eşit olmalıdır.");
    return -p.reduce((acc, pi, i) => {
        if (pi === 0) return acc;
        if (q[i] <= 0) throw new RangeError("caprazEntropi: q değerleri > 0 olmalıdır.");
        return acc + pi * Math.log(q[i]);
    }, 0);
}

/**
 * KL Iraksaması (Kullback-Leibler) — D_KL(P || Q)
 * D_KL = Σ p(x) * log(p(x) / q(x))
 * @param {number[]} p
 * @param {number[]} q
 * @returns {number}
 */
function klIraksamasi(p, q) {
    if (p.length !== q.length) throw new Error("klIraksamasi: Dizi uzunlukları eşit olmalıdır.");
    return p.reduce((acc, pi, i) => {
        if (pi === 0) return acc;
        if (q[i] <= 0) throw new RangeError("klIraksamasi: q değerleri > 0 olmalıdır.");
        return acc + pi * Math.log(pi / q[i]);
    }, 0);
}

/**
 * Jensen-Shannon ıraksaması — JSD(P || Q)
 * JSD = (D_KL(P||M) + D_KL(Q||M)) / 2, M = (P + Q) / 2
 * @param {number[]} p
 * @param {number[]} q
 * @returns {number}
 */
function jsIraksamasi(p, q) {
    if (p.length !== q.length) throw new Error("jsIraksamasi: Dizi uzunlukları eşit olmalıdır.");
    const m = p.map((pi, i) => (pi + q[i]) / 2);
    return (klIraksamasi(p, m) + klIraksamasi(q, m)) / 2;
}

/**
 * Karşılıklı Bilgi — I(X;Y) = H(X) + H(Y) - H(X,Y)
 * @param {number[]} px — X marjinal dağılımı
 * @param {number[]} py — Y marjinal dağılımı
 * @param {number[][]} pxy — birleşik dağılım
 * @returns {number}
 */
function karsilikliDeger(px, py, pxy) {
    let mi = 0;
    for (let i = 0; i < px.length; i++) {
        for (let j = 0; j < py.length; j++) {
            const pij = pxy[i][j];
            if (pij > 0) {
                mi += pij * Math.log2(pij / (px[i] * py[j]));
            }
        }
    }
    return mi;
}

// =============================================================================
// BÖLÜM 6 — ÖZEL LOGARİTMİK FONKSİYONLAR
// =============================================================================

/**
 * Logit fonksiyonu — logit(p) = ln(p / (1 - p))
 * Lojistik regresyon ve bilgi teorisinde kullanılır.
 * @param {number} p — 0 < p < 1
 * @returns {number}
 */
function logit(p) {
    if (p <= 0 || p >= 1) throw new RangeError(`logit: 0 < p < 1 olmalıdır, p = ${p}`);
    return Math.log(p / (1 - p));
}

/**
 * Sigmoid (ters logit) — σ(x) = 1 / (1 + e^(-x))
 * @param {number} x
 * @returns {number}
 */
function sigmoid(x) {
    return 1 / (1 + Math.exp(-x));
}

/**
 * Log-sigmoid — ln(σ(x))
 * Sayısal kararlılık için optimize edilmiş.
 * @param {number} x
 * @returns {number}
 */
function logSigmoid(x) {
    if (x >= 0) {
        return -Math.log1p(Math.exp(-x));
    }
    return x - Math.log1p(Math.exp(x));
}

/**
 * Softplus — ln(1 + e^x)
 * ReLU'nun yumuşatılmış versiyonu.
 * @param {number} x
 * @returns {number}
 */
function softplus(x) {
    if (x > 20) return x;
    if (x < -20) return Math.exp(x);
    return Math.log1p(Math.exp(x));
}

/**
 * LogSumExp — ln(Σ e^{x_i})
 * Sayısal taşmayı önleyen kararlı uygulama.
 * @param {number[]} x — sayı dizisi
 * @returns {number}
 */
function logSumExp(x) {
    if (x.length === 0) throw new Error("logSumExp: Boş dizi.");
    const maksimum = Math.max(...x);
    if (!isFinite(maksimum)) return maksimum;
    const toplam = x.reduce((acc, xi) => acc + Math.exp(xi - maksimum), 0);
    return maksimum + Math.log(toplam);
}

/**
 * Log-Gamma fonksiyonu — ln(Γ(x))
 * Stirling yaklaşımı kullanır.
 * @param {number} x — x > 0
 * @returns {number}
 */
function logGamma(x) {
    if (x <= 0) throw new RangeError("logGamma: x > 0 olmalıdır.");
    // Lanczos yaklaşımı
    const g = 7;
    const c = [
        0.99999999999980993,
        676.5203681218851,
        -1259.1392167224028,
        771.32342877765313,
        -176.61502916214059,
        12.507343278686905,
        -0.13857109526572012,
        9.9843695780195716e-6,
        1.5056327351493116e-7
    ];
    if (x < 0.5) {
        return Math.log(PI / Math.sin(PI * x)) - logGamma(1 - x);
    }
    const xAz = x - 1;
    let a = c[0];
    const t = xAz + g + 0.5;
    for (let i = 1; i < g + 2; i++) {
        a += c[i] / (xAz + i);
    }
    return 0.5 * Math.log(2 * PI) + (xAz + 0.5) * Math.log(t) - t + Math.log(a);
}

/**
 * Digamma fonksiyonu — ψ(x) = d/dx ln(Γ(x))
 * @param {number} x — x > 0
 * @returns {number}
 */
function digamma(x) {
    if (x <= 0 && Number.isInteger(x)) {
        throw new RangeError("digamma: x negatif tam sayı olamaz.");
    }
    // Büyük x için asimptotik açılım
    if (x < 6) {
        return digamma(x + 1) - 1 / x;
    }
    const xInv = 1 / x;
    const xInv2 = xInv * xInv;
    return Math.log(x) - 0.5 * xInv
        - xInv2 * (1 / 12 - xInv2 * (1 / 120 - xInv2 * (1 / 252 - xInv2 / 240)));
}

/**
 * Logaritmik integral — li(x) = ∫₀ˣ dt / ln(t) (Cauchy P.V.)
 * Sayı teorisinde kullanılır.
 * @param {number} x — x > 0, x ≠ 1
 * @returns {number}
 */
function logaritmikIntegral(x) {
    if (x <= 0) throw new RangeError("logaritmikIntegral: x > 0 olmalıdır.");
    if (x === 1) throw new RangeError("logaritmikIntegral: x = 1'de tanımsız.");
    if (x < 1) {
        // Küçük x için yaklaşım
        const lnx = Math.log(x);
        return GAMMA_EM + Math.log(Math.abs(lnx)) +
            Array.from({ length: 20 }, (_, n) => Math.pow(lnx, n + 1) / (factorial(n + 1) * (n + 1)))
                .reduce((a, b) => a + b, 0);
    }
    // Ramanujan serisi yaklaşımı
    const lnx = Math.log(x);
    let sonuc = GAMMA_EM + Math.log(lnx);
    let terim = 1;
    for (let n = 1; n <= 50; n++) {
        terim *= lnx / n;
        sonuc += terim / n;
    }
    return sonuc;
}

// =============================================================================
// BÖLÜM 7 — LOGARİTMİK ÖLÇEK VE DÖNÜŞÜMLER
// =============================================================================

/**
 * Desibel dönüşümü — dB = 20 * log₁₀(amplitüd / referans)
 * @param {number} amplitud   — ölçülen değer
 * @param {number} referans   — referans değeri (varsayılan: 1)
 * @returns {number} dB cinsinden değer
 */
function desibelAmplitud(amplitud, referans = 1) {
    if (amplitud <= 0 || referans <= 0) {
        throw new RangeError("desibelAmplitud: Pozitif değerler gereklidir.");
    }
    return 20 * Math.log10(amplitud / referans);
}

/**
 * Desibel dönüşümü — güç için dB = 10 * log₁₀(güç / referans)
 * @param {number} guc       — ölçülen güç
 * @param {number} referans  — referans gücü
 * @returns {number} dBW veya dBm cinsinden değer
 */
function desibelGuc(guc, referans = 1) {
    if (guc <= 0 || referans <= 0) {
        throw new RangeError("desibelGuc: Pozitif değerler gereklidir.");
    }
    return 10 * Math.log10(guc / referans);
}

/**
 * Desibelden doğrusal ölçeğe — amplitüd
 * @param {number} dB
 * @param {number} referans
 * @returns {number}
 */
function dBdenAmplitud(dB, referans = 1) {
    return referans * Math.pow(10, dB / 20);
}

/**
 * Desibelden doğrusal ölçeğe — güç
 * @param {number} dB
 * @param {number} referans
 * @returns {number}
 */
function dBdenGuc(dB, referans = 1) {
    return referans * Math.pow(10, dB / 10);
}

/**
 * pH hesabı — pH = -log₁₀([H⁺])
 * @param {number} hKonstrasyonu — mol/L cinsinden H⁺ konsantrasyonu
 * @returns {number}
 */
function pH(hKonstrasyonu) {
    if (hKonstrasyonu <= 0) throw new RangeError("pH: H⁺ konsantrasyonu > 0 olmalıdır.");
    return -Math.log10(hKonstrasyonu);
}

/**
 * pH'den H⁺ konsantrasyonuna
 * @param {number} phDegeri
 * @returns {number} mol/L
 */
function pHdenKonstrasyon(phDegeri) {
    return Math.pow(10, -phDegeri);
}

/**
 * Richter ölçeği — M = log₁₀(A / A₀)
 * @param {number} amplitud   — deprem dalgası genliği (μm)
 * @param {number} referans   — referans genlik (varsayılan: 1 μm)
 * @returns {number}
 */
function richterMagnitude(amplitud, referans = 1) {
    if (amplitud <= 0) throw new RangeError("richterMagnitude: amplitud > 0 olmalıdır.");
    return Math.log10(amplitud / referans);
}

/**
 * Büyüklükten enerji hesabı — E = 10^(1.5M + 4.8) joule
 * @param {number} buyukluk — Richter büyüklüğü
 * @returns {number} joule
 */
function depremEnerjisi(buyukluk) {
    return Math.pow(10, 1.5 * buyukluk + 4.8);
}

/**
 * Stellar (yıldız) büyüklük farkı — Δm = -2.5 * log₁₀(F1/F2)
 * @param {number} f1 — birinci yıldızın akısı
 * @param {number} f2 — ikinci yıldızın akısı
 * @returns {number}
 */
function stellarBuyukluk(f1, f2) {
    if (f1 <= 0 || f2 <= 0) throw new RangeError("stellarBuyukluk: f1, f2 > 0 olmalıdır.");
    return -2.5 * Math.log10(f1 / f2);
}

// =============================================================================
// BÖLÜM 8 — İSTATİSTİKSEL LOGARİTMİK DAĞILIMLAR
// =============================================================================

/**
 * Log-normal dağılım — PDF
 * f(x; μ, σ) = 1/(xσ√(2π)) * exp(-(ln(x)-μ)² / (2σ²))
 * @param {number} x  — x > 0
 * @param {number} mu — konum parametresi
 * @param {number} sigma — ölçek parametresi (σ > 0)
 * @returns {number}
 */
function logNormalPDF(x, mu = 0, sigma = 1) {
    if (x <= 0) throw new RangeError("logNormalPDF: x > 0 olmalıdır.");
    if (sigma <= 0) throw new RangeError("logNormalPDF: sigma > 0 olmalıdır.");
    const lnx = Math.log(x);
    const z = (lnx - mu) / sigma;
    return Math.exp(-0.5 * z * z) / (x * sigma * Math.sqrt(2 * PI));
}

/**
 * Log-normal dağılım — CDF
 * F(x) = Φ((ln(x) - μ) / σ)
 * @param {number} x
 * @param {number} mu
 * @param {number} sigma
 * @returns {number}
 */
function logNormalCDF(x, mu = 0, sigma = 1) {
    if (x <= 0) return 0;
    const z = (Math.log(x) - mu) / sigma;
    return normalCDF(z);
}

/**
 * Standart normal CDF — Φ(x) ≈ (yardımcı)
 * @param {number} x
 * @returns {number}
 */
function normalCDF(x) {
    const t = 1 / (1 + 0.2315419 * Math.abs(x));
    const d = 0.3989422820 * Math.exp(-x * x / 2);
    let prob = d * t * (0.3193815 + t * (-0.3565638 +
        t * (1.7814780 + t * (-1.8212560 + t * 1.3302744))));
    return x > 0 ? 1 - prob : prob;
}

/**
 * Log-normal dağılım — ortalama
 * E[X] = exp(μ + σ²/2)
 * @param {number} mu
 * @param {number} sigma
 * @returns {number}
 */
function logNormalOrtalama(mu, sigma) {
    return Math.exp(mu + sigma * sigma / 2);
}

/**
 * Log-normal dağılım — medyan
 * Medyan = exp(μ)
 * @param {number} mu
 * @returns {number}
 */
function logNormalMedian(mu) {
    return Math.exp(mu);
}

/**
 * Log-normal dağılım — varyans
 * Var[X] = (exp(σ²) - 1) * exp(2μ + σ²)
 * @param {number} mu
 * @param {number} sigma
 * @returns {number}
 */
function logNormalVaryans(mu, sigma) {
    const s2 = sigma * sigma;
    return (Math.exp(s2) - 1) * Math.exp(2 * mu + s2);
}

/**
 * Log-lojistik dağılım — PDF
 * f(x; α, β) = (β/α)(x/α)^(β-1) / (1 + (x/α)^β)²
 * @param {number} x  — x > 0
 * @param {number} alpha — ölçek
 * @param {number} beta  — şekil
 * @returns {number}
 */
function logLojistikPDF(x, alpha = 1, beta = 1) {
    if (x <= 0) return 0;
    const oran = x / alpha;
    const t = Math.pow(oran, beta);
    return (beta / alpha) * (t / oran) / Math.pow(1 + t, 2);
}

// =============================================================================
// BÖLÜM 9 — KOMBİNATORA VE SAYI TEORİSİ
// =============================================================================

/**
 * Faktöriyel (iteratif)
 * @param {number} n — negatif olmayan tam sayı
 * @returns {number}
 */
function factorial(n) {
    if (!Number.isInteger(n) || n < 0) {
        throw new RangeError(`factorial: Negatif olmayan tam sayı gerekli, n = ${n}`);
    }
    if (n === 0 || n === 1) return 1;
    let sonuc = 1;
    for (let i = 2; i <= n; i++) sonuc *= i;
    return sonuc;
}

/**
 * Stirling yaklaşımı — ln(n!) ≈ n*ln(n) - n + 0.5*ln(2πn)
 * @param {number} n
 * @returns {number}
 */
function stirlingLog(n) {
    if (n < 0) throw new RangeError("stirlingLog: n ≥ 0 olmalıdır.");
    if (n === 0) return 0;
    return n * Math.log(n) - n + 0.5 * Math.log(2 * PI * n);
}

/**
 * Logaritmik binomial katsayı — ln(C(n, k))
 * @param {number} n
 * @param {number} k
 * @returns {number}
 */
function logBinom(n, k) {
    if (k < 0 || k > n) return -Infinity;
    return logGamma(n + 1) - logGamma(k + 1) - logGamma(n - k + 1);
}

/**
 * Logaritmik permütasyon — ln(P(n, k)) = ln(n! / (n-k)!)
 * @param {number} n
 * @param {number} k
 * @returns {number}
 */
function logPermutasyon(n, k) {
    if (k < 0 || k > n) return -Infinity;
    return logGamma(n + 1) - logGamma(n - k + 1);
}

/**
 * İlkel kökler sayısı — φ(n) için log yaklaşımı
 * ln(φ(n)) ≈ ln(n) + Σ_{p|n} ln(1 - 1/p)
 * @param {number} n
 * @returns {number}
 */
function logEulerPhi(n) {
    if (n <= 0) throw new RangeError("logEulerPhi: n > 0 olmalıdır.");
    let lnPhi = Math.log(n);
    let temp = n;
    for (let p = 2; p * p <= temp; p++) {
        if (temp % p === 0) {
            lnPhi += Math.log1p(-1 / p);
            while (temp % p === 0) temp = Math.floor(temp / p);
        }
    }
    if (temp > 1) lnPhi += Math.log1p(-1 / temp);
    return lnPhi;
}

// =============================================================================
// BÖLÜM 10 — KARMAŞIK LOGARİTMA
// =============================================================================

/**
 * Karmaşık sayılar için Log hesabı
 * @typedef {{re: number, im: number}} Karmasik
 */

/**
 * Karmaşık logaritma — ln(a + bi)
 * ln(z) = ln|z| + i*arg(z)
 * @param {number} re — gerçel kısım
 * @param {number} im — sanal kısım
 * @returns {Karmasik}
 */
function karmasikLog(re, im) {
    if (re === 0 && im === 0) {
        throw new RangeError("karmasikLog: z = 0 için tanımsız.");
    }
    return {
        re: 0.5 * Math.log(re * re + im * im),
        im: Math.atan2(im, re)
    };
}

/**
 * Karmaşık üs — e^(a + bi) = e^a * (cos(b) + i*sin(b))
 * @param {number} re
 * @param {number} im
 * @returns {Karmasik}
 */
function karmasikExp(re, im) {
    const eFactor = Math.exp(re);
    return {
        re: eFactor * Math.cos(im),
        im: eFactor * Math.sin(im)
    };
}

/**
 * Karmaşık sayı tabanlı güç — z^w = e^(w * ln(z))
 * @param {number} zRe — z gerçel
 * @param {number} zIm — z sanal
 * @param {number} wRe — w gerçel
 * @param {number} wIm — w sanal
 * @returns {Karmasik}
 */
function karmasikGuc(zRe, zIm, wRe, wIm) {
    const lnZ = karmasikLog(zRe, zIm);
    const wLnZ = {
        re: wRe * lnZ.re - wIm * lnZ.im,
        im: wRe * lnZ.im + wIm * lnZ.re
    };
    return karmasikExp(wLnZ.re, wLnZ.im);
}

/**
 * Polar form — ln(r * e^{iθ}) = ln(r) + iθ
 * @param {number} r  — modül
 * @param {number} theta — argüman (radyan)
 * @returns {Karmasik}
 */
function polarLog(r, theta) {
    if (r <= 0) throw new RangeError("polarLog: r > 0 olmalıdır.");
    return { re: Math.log(r), im: theta };
}

// =============================================================================
// BÖLÜM 11 — LOGARİTMİK TÜREV VE İNTEGRAL
// =============================================================================

/**
 * Logaritmik türev — d/dx[ln(f(x))] = f'(x) / f(x)
 * Sayısal hesaplama (merkezi fark yöntemi).
 * @param {Function} f — fonksiyon
 * @param {number}   x — nokta
 * @param {number}   h — adım boyutu
 * @returns {number}
 */
function logaritmikTurev(f, x, h = 1e-7) {
    const fxh1 = f(x + h);
    const fxh2 = f(x - h);
    const turev = (fxh1 - fxh2) / (2 * h);
    return turev / f(x);
}

/**
 * İkinci logaritmik türev — d²/dx²[ln(f(x))]
 * = (f''f - (f')²) / f²
 * @param {Function} f
 * @param {number}   x
 * @param {number}   h
 * @returns {number}
 */
function logaritmikIkinciTurev(f, x, h = 1e-5) {
    const fx = f(x);
    const fxp = f(x + h);
    const fxm = f(x - h);
    const fTurev = (fxp - fxm) / (2 * h);
    const fTurev2 = (fxp - 2 * fx + fxm) / (h * h);
    return (fTurev2 * fx - fTurev * fTurev) / (fx * fx);
}

/**
 * Sayısal logaritmik integral — ∫ ln(x) dx = x*ln(x) - x + C
 * @param {number} x
 * @returns {number} x*ln(x) - x
 */
function logIntegralAnalitik(x) {
    if (x <= 0) throw new RangeError("logIntegralAnalitik: x > 0 olmalıdır.");
    return x * Math.log(x) - x;
}

/**
 * ∫_a^b ln(x) dx hesabı
 * @param {number} a — alt sınır (> 0)
 * @param {number} b — üst sınır
 * @returns {number}
 */
function logIntegralBelirli(a, b) {
    if (a <= 0) throw new RangeError("logIntegralBelirli: a > 0 olmalıdır.");
    return logIntegralAnalitik(b) - logIntegralAnalitik(a);
}

/**
 * Simpson yöntemi ile sayısal integral — ∫_a^b f(x) dx
 * @param {Function} f — integrand
 * @param {number}   a — alt sınır
 * @param {number}   b — üst sınır
 * @param {number}   n — bölüm sayısı (çift)
 * @returns {number}
 */
function simpsonIntegral(f, a, b, n = 1000) {
    if (n % 2 !== 0) n++;
    const h = (b - a) / n;
    let toplam = f(a) + f(b);
    for (let i = 1; i < n; i++) {
        const xi = a + i * h;
        toplam += (i % 2 === 0 ? 2 : 4) * f(xi);
    }
    return (h / 3) * toplam;
}

// =============================================================================
// BÖLÜM 12 — LOGARİTMİK BÜYÜME VE EKONOMİ
// =============================================================================

/**
 * Bileşik faiz — A = P(1 + r/n)^(nt)
 * @param {number} anapara  — başlangıç değeri
 * @param {number} oran      — yıllık faiz oranı (ondalık)
 * @param {number} n         — yılda bileşim sayısı
 * @param {number} sure      — süre (yıl)
 * @returns {number}
 */
function bilesikFaiz(anapara, oran, n, sure) {
    return anapara * Math.pow(1 + oran / n, n * sure);
}

/**
 * Sürekli bileşik faiz — A = P * e^(rt)
 * @param {number} anapara
 * @param {number} oran
 * @param {number} sure
 * @returns {number}
 */
function sureklibilesikFaiz(anapara, oran, sure) {
    return anapara * Math.exp(oran * sure);
}

/**
 * İkiye katlama süresi — t = ln(2) / r
 * @param {number} oran — büyüme oranı
 * @returns {number} yıl cinsinden
 */
function ikiyeKatlamaSuresi(oran) {
    if (oran <= 0) throw new RangeError("ikiyeKatlamaSuresi: oran > 0 olmalıdır.");
    return LN2 / oran;
}

/**
 * Yarılanma süresi — t_½ = -ln(2) / λ (bozunma sabiti)
 * @param {number} lambda — bozunma sabiti
 * @returns {number}
 */
function yarilaniSuresi(lambda) {
    if (lambda <= 0) throw new RangeError("yarilaniSuresi: lambda > 0 olmalıdır.");
    return LN2 / lambda;
}

/**
 * Radyoaktif bozunma — N(t) = N₀ * e^(-λt)
 * @param {number} n0     — başlangıç miktarı
 * @param {number} lambda — bozunma sabiti
 * @param {number} sure   — süre
 * @returns {number}
 */
function radyoaktifBozunma(n0, lambda, sure) {
    return n0 * Math.exp(-lambda * sure);
}

/**
 * Logaritmik getiri — r = ln(P_t / P_{t-1})
 * Finans alanında kullanılan sürekli bileşik getiri.
 * @param {number} ptOnceki — önceki fiyat
 * @param {number} ptSon    — son fiyat
 * @returns {number}
 */
function logaritmikGetiri(ptOnceki, ptSon) {
    if (ptOnceki <= 0 || ptSon <= 0) {
        throw new RangeError("logaritmikGetiri: Fiyatlar > 0 olmalıdır.");
    }
    return Math.log(ptSon / ptOnceki);
}

/**
 * Kümülatif logaritmik getiri — sum of log returns
 * @param {number[]} fiyatlar — fiyat serisi
 * @returns {number}
 */
function kumulatifLogGetiri(fiyatlar) {
    if (fiyatlar.length < 2) throw new Error("kumulatifLogGetiri: En az 2 fiyat gereklidir.");
    let toplam = 0;
    for (let i = 1; i < fiyatlar.length; i++) {
        toplam += logaritmikGetiri(fiyatlar[i - 1], fiyatlar[i]);
    }
    return toplam;
}

/**
 * Volatilite (log-getiri standart sapması) hesabı
 * @param {number[]} fiyatlar — fiyat serisi
 * @returns {number}
 */
function volatilite(fiyatlar) {
    const getiriler = [];
    for (let i = 1; i < fiyatlar.length; i++) {
        getiriler.push(logaritmikGetiri(fiyatlar[i - 1], fiyatlar[i]));
    }
    const ortalama = getiriler.reduce((a, b) => a + b, 0) / getiriler.length;
    const varyans = getiriler.reduce((acc, r) => acc + Math.pow(r - ortalama, 2), 0) / (getiriler.length - 1);
    return Math.sqrt(varyans);
}

// =============================================================================
// BÖLÜM 13 — HESAPLAMA ARAÇLARI VE TABLOLAR
// =============================================================================

/**
 * Logaritma tablosu oluşturur
 * @param {number}   baslangic — başlangıç değeri
 * @param {number}   bitis     — bitiş değeri
 * @param {number}   adim      — adım boyutu
 * @param {number[]} tabanlar  — hesaplanacak tabanlar
 * @returns {Array<{x: number, ln: number, log2: number, log10: number}>}
 */
function logaritmTablosu(baslangic = 1, bitis = 10, adim = 1, tabanlar = [E, 2, 10]) {
    const tablo = [];
    for (let x = baslangic; x <= bitis + 1e-10; x += adim) {
        const satir = { x: parseFloat(x.toFixed(6)) };
        tabanlar.forEach(t => {
            const anahtar = t === E ? "ln" : `log${Number.isInteger(t) ? t : t.toFixed(2)}`;
            satir[anahtar] = logTaban(x, t);
        });
        tablo.push(satir);
    }
    return tablo;
}

/**
 * İki sayının logaritmik interpolasyonu
 * log_b(x) için a ≤ x ≤ b arasında lineer interpolasyon
 * @param {number} x      — hedef değer
 * @param {number} x1     — sol sınır
 * @param {number} x2     — sağ sınır
 * @param {number} taban  — logaritma tabanı
 * @returns {number}
 */
function logInterpolasyon(x, x1, x2, taban = E) {
    if (x < x1 || x > x2) throw new RangeError("logInterpolasyon: x, [x1, x2] aralığında olmalıdır.");
    const y1 = logTaban(x1, taban);
    const y2 = logTaban(x2, taban);
    const t = (x - x1) / (x2 - x1);
    return y1 + t * (y2 - y1);
}

/**
 * Chebyshev node'ları ile log interpolasyonu
 * [a, b] aralığında Chebyshev dağılımı
 * @param {number} a    — alt sınır
 * @param {number} b    — üst sınır
 * @param {number} n    — node sayısı
 * @param {number} taban
 * @returns {{nokta: number, deger: number}[]}
 */
function chebyshevLogNodelar(a, b, n = 10, taban = E) {
    return Array.from({ length: n }, (_, k) => {
        const x = (a + b) / 2 + (b - a) / 2 * Math.cos((2 * k + 1) * PI / (2 * n));
        return { nokta: x, deger: logTaban(x, taban) };
    });
}

/**
 * Logaritmik ölçekte n nokta üret (log-space)
 * @param {number} baslangic — başlangıç üs
 * @param {number} bitis     — bitiş üs
 * @param {number} n         — nokta sayısı
 * @param {number} taban     — taban
 * @returns {number[]}
 */
function logSpace(baslangic, bitis, n = 50, taban = 10) {
    const sonuclar = [];
    const adim = (bitis - baslangic) / (n - 1);
    for (let i = 0; i < n; i++) {
        sonuclar.push(Math.pow(taban, baslangic + i * adim));
    }
    return sonuclar;
}

// =============================================================================
// BÖLÜM 14 — DOĞRULAMA VE HATA KONTROLÜ
// =============================================================================

/**
 * Log hesabı için girdi doğrulaması
 * @param {*} x
 * @param {string} fonksiyonAdi
 * @returns {{gecerli: boolean, hata: string|null}}
 */
function logDogrulama(x, fonksiyonAdi = "log") {
    if (x === null || x === undefined) {
        return { gecerli: false, hata: `${fonksiyonAdi}: null/undefined değer.` };
    }
    if (typeof x !== "number") {
        return { gecerli: false, hata: `${fonksiyonAdi}: Sayısal değer gerekli, ${typeof x} alındı.` };
    }
    if (isNaN(x)) {
        return { gecerli: false, hata: `${fonksiyonAdi}: NaN değeri.` };
    }
    if (!isFinite(x)) {
        return { gecerli: false, hata: `${fonksiyonAdi}: Sonsuz değer.` };
    }
    if (x <= 0) {
        return { gecerli: false, hata: `${fonksiyonAdi}: x > 0 gerekli, x = ${x}` };
    }
    return { gecerli: true, hata: null };
}

/**
 * Güvenli logaritma — hata fırlatmadan hesapla
 * @param {number} x
 * @param {number} taban
 * @param {number} varsayilan — geçersiz girdi durumunda döndürülecek değer
 * @returns {number}
 */
function guvenliLog(x, taban = E, varsayilan = NaN) {
    try {
        const dogrulama = logDogrulama(x, "guvenliLog");
        if (!dogrulama.gecerli) return varsayilan;
        return logTaban(x, taban);
    } catch {
        return varsayilan;
    }
}

/**
 * Logaritma hassasiyet testi — yaklaşık eşitlik
 * @param {number} hesaplanan
 * @param {number} beklenen
 * @param {number} tolerans
 * @returns {{basarili: boolean, mutlakHata: number, gorecekHata: number}}
 */
function logHassasiyetTest(hesaplanan, beklenen, tolerans = 1e-10) {
    const mutlakHata = Math.abs(hesaplanan - beklenen);
    const gorecekHata = beklenen !== 0 ? mutlakHata / Math.abs(beklenen) : mutlakHata;
    return {
        basarili: mutlakHata < tolerans,
        mutlakHata,
        gorecekHata
    };
}

// =============================================================================
// BÖLÜM 15 — VERİ ANALİZİ VE İSTATİSTİK
// =============================================================================

/**
 * Verinin logaritmik dönüşümü
 * @param {number[]} veri
 * @param {number}   taban
 * @returns {number[]}
 */
function logDonusum(veri, taban = E) {
    return veri.map(x => {
        if (x <= 0) throw new RangeError(`logDonusum: Tüm değerler > 0 olmalı, ${x} bulundu.`);
        return logTaban(x, taban);
    });
}

/**
 * Box-Cox dönüşümü — y(λ) = (x^λ - 1)/λ  (λ ≠ 0) veya ln(x) (λ = 0)
 * @param {number[]} veri
 * @param {number}   lambda
 * @returns {number[]}
 */
function boxCoxDonusum(veri, lambda = 0) {
    return veri.map(x => {
        if (x <= 0) throw new RangeError("boxCoxDonusum: Tüm değerler > 0 olmalıdır.");
        if (Math.abs(lambda) < 1e-10) return Math.log(x);
        return (Math.pow(x, lambda) - 1) / lambda;
    });
}

/**
 * Logaritmik regresyon — y = a + b * ln(x)
 * En küçük kareler yöntemi ile a ve b katsayılarını hesaplar.
 * @param {number[]} x — bağımsız değişken
 * @param {number[]} y — bağımlı değişken
 * @returns {{a: number, b: number, r2: number}}
 */
function logaritmikRegresyon(x, y) {
    if (x.length !== y.length) throw new Error("logaritmikRegresyon: Dizi uzunlukları eşit olmalı.");
    const n = x.length;
    const lnX = x.map(xi => {
        if (xi <= 0) throw new RangeError("logaritmikRegresyon: x değerleri > 0 olmalı.");
        return Math.log(xi);
    });
    const sumLnX = lnX.reduce((a, b) => a + b, 0);
    const sumY = y.reduce((a, b) => a + b, 0);
    const sumLnX2 = lnX.reduce((acc, l) => acc + l * l, 0);
    const sumLnXY = lnX.reduce((acc, l, i) => acc + l * y[i], 0);
    const b = (n * sumLnXY - sumLnX * sumY) / (n * sumLnX2 - sumLnX * sumLnX);
    const a = (sumY - b * sumLnX) / n;
    // R² hesapla
    const yOrt = sumY / n;
    const ssTot = y.reduce((acc, yi) => acc + Math.pow(yi - yOrt, 2), 0);
    const ssRes = y.reduce((acc, yi, i) => acc + Math.pow(yi - (a + b * lnX[i]), 2), 0);
    const r2 = 1 - ssRes / ssTot;
    return { a, b, r2 };
}

/**
 * Log-lineer model tahmini
 * @param {number}   a — sabit
 * @param {number}   b — eğim
 * @param {number}   x — bağımsız değer
 * @returns {number}
 */
function logLineerTahmin(a, b, x) {
    if (x <= 0) throw new RangeError("logLineerTahmin: x > 0 olmalıdır.");
    return a + b * Math.log(x);
}

/**
 * Veri setinin çarpıklığını (skewness) log-dönüşüm ile azalt
 * @param {number[]} veri
 * @returns {{donusturulmus: number[], lambda: number}}
 */
function carpiklıkDuzeltme(veri) {
    // Optimal lambda -2 ile 2 arasında aranır
    let enIyiLambda = 0;
    let enKucukHata = Infinity;
    for (let lambda = -2; lambda <= 2; lambda += 0.1) {
        try {
            const donusturulmus = boxCoxDonusum(veri, lambda);
            const ortalama = donusturulmus.reduce((a, b) => a + b, 0) / donusturulmus.length;
            const carpiklik = donusturulmus.reduce((acc, x) =>
                acc + Math.pow(x - ortalama, 3), 0) / donusturulmus.length;
            if (Math.abs(carpiklik) < enKucukHata) {
                enKucukHata = Math.abs(carpiklik);
                enIyiLambda = lambda;
            }
        } catch { /* geçersiz lambda atla */ }
    }
    return {
        donusturulmus: boxCoxDonusum(veri, enIyiLambda),
        lambda: enIyiLambda
    };
}

// =============================================================================
// BÖLÜM 16 — HESAP MAKİNESİ ARAYÜZÜ
// =============================================================================

/**
 * Logaritma hesap makinesi — tek giriş noktası
 * @param {number}  x     — girdi değeri
 * @param {string}  tip   — "ln" | "log2" | "log10" | "custom"
 * @param {number}  taban — tip "custom" ise kullanılan taban
 * @returns {{sonuc: number, ifade: string, yontem: string}}
 */
function hesapla(x, tip = "ln", taban = E) {
    const dogrulama = logDogrulama(x, "hesapla");
    if (!dogrulama.gecerli) throw new Error(dogrulama.hata);
    let sonuc, ifade, yontem;
    switch (tip.toLowerCase()) {
        case "ln":
            sonuc = ln(x);
            ifade = `ln(${x})`;
            yontem = "Doğal logaritma";
            break;
        case "log2":
            sonuc = log2(x);
            ifade = `log₂(${x})`;
            yontem = "İkili logaritma";
            break;
        case "log10":
            sonuc = log10(x);
            ifade = `log₁₀(${x})`;
            yontem = "Onluk logaritma";
            break;
        case "custom":
            sonuc = logTaban(x, taban);
            ifade = `log_${taban}(${x})`;
            yontem = `${taban} tabanında logaritma`;
            break;
        default:
            throw new Error(`hesapla: Bilinmeyen tip "${tip}". "ln", "log2", "log10", "custom" kullanın.`);
    }
    return { sonuc, ifade, yontem };
}

/**
 * Toplu hesaplama — bir dizi değer için logaritma
 * @param {number[]} dizi
 * @param {string}   tip
 * @param {number}   taban
 * @returns {Array<{x: number, sonuc: number, gecerli: boolean}>}
 */
function topluHesapla(dizi, tip = "ln", taban = E) {
    return dizi.map(x => {
        try {
            return { x, sonuc: hesapla(x, tip, taban).sonuc, gecerli: true };
        } catch (e) {
            return { x, sonuc: null, gecerli: false, hata: e.message };
        }
    });
}

// =============================================================================
// BÖLÜM 17 — DIŞA AKTARMA (EXPORT)
// =============================================================================

const LogaritmaKutuphanesi = {
    // Sabitler
    sabitler: { E, PI, LN2, LN10, LOG2E, LOG10E, GAMMA_EM, PHI, LN_PHI, LN_2PI, EPSILON },
    // Temel
    ln, log2, log10, logTaban, log1p, negatifLog, logMutlak, isareliLog,
    tavanLog, tabanLog,
    // Özellikler
    carpimKurali, bolmeKurali, usKurali, tabanDegisimi, tersiLog,
    logaritmikOrtalama, geometrikOrtalama, geometrikOrtalamaArray,
    // Seri
    taylorLog1p, mercatorLog, agmLog, newtonLog, halleyLog,
    // Bilgi teorisi
    ikililiEntropi, shannonEntropi, caprazEntropi, klIraksamasi,
    jsIraksamasi, karsilikliDeger,
    // Özel
    logit, sigmoid, logSigmoid, softplus, logSumExp, logGamma, digamma,
    logaritmikIntegral,
    // Ölçek
    desibelAmplitud, desibelGuc, dBdenAmplitud, dBdenGuc,
    pH, pHdenKonstrasyon, richterMagnitude, depremEnerjisi, stellarBuyukluk,
    // İstatistik
    logNormalPDF, logNormalCDF, logNormalOrtalama, logNormalMedian,
    logNormalVaryans, logLojistikPDF,
    // Kombinatör
    factorial, stirlingLog, logBinom, logPermutasyon, logEulerPhi,
    // Karmaşık
    karmasikLog, karmasikExp, karmasikGuc, polarLog,
    // Türev / integral
    logaritmikTurev, logaritmikIkinciTurev, logIntegralAnalitik,
    logIntegralBelirli, simpsonIntegral,
    // Ekonomi
    bilesikFaiz, sureklibilesikFaiz, ikiyeKatlamaSuresi, yarilaniSuresi,
    radyoaktifBozunma, logaritmikGetiri, kumulatifLogGetiri, volatilite,
    // Araçlar
    logaritmTablosu, logInterpolasyon, chebyshevLogNodelar, logSpace,
    // Doğrulama
    logDogrulama, guvenliLog, logHassasiyetTest,
    // Analiz
    logDonusum, boxCoxDonusum, logaritmikRegresyon, logLineerTahmin,
    carpiklıkDuzeltme,
    // Arayüz
    hesapla, topluHesapla
};

// Node.js / CommonJS
if (typeof module !== "undefined" && module.exports) {
    module.exports = LogaritmaKutuphanesi;
}

// ESM
if (typeof window !== "undefined") {
    window.LogaritmaKutuphanesi = LogaritmaKutuphanesi;
}

// =============================================================================
// BÖLÜM 18 — KULLANIM ÖRNEKLERİ
// =============================================================================
/*
  // --- Temel Kullanım ---
  const { ln, log2, log10, logTaban } = LogaritmaKutuphanesi;

  console.log(ln(Math.E));           // 1
  console.log(log2(1024));           // 10
  console.log(log10(1000));          // 3
  console.log(logTaban(8, 2));       // 3

  // --- Bilgi Teorisi ---
  const { shannonEntropi, klIraksamasi } = LogaritmaKutuphanesi;

  const dagilim1 = [0.5, 0.25, 0.25];
  console.log(shannonEntropi(dagilim1));  // ~1.5 bit

  // --- Finansal Hesaplama ---
  const { logaritmikGetiri, volatilite } = LogaritmaKutuphanesi;

  const fiyatlar = [100, 105, 102, 110, 108];
  console.log(logaritmikGetiri(100, 108));     // ~0.0770
  console.log(volatilite(fiyatlar));           // günlük vol

  // --- Fizik / Kimya ---
  const { pH, desibelAmplitud, richterMagnitude } = LogaritmaKutuphanesi;

  console.log(pH(1e-7));                   // 7 (saf su)
  console.log(desibelAmplitud(20, 1));     // ~26.02 dB
  console.log(richterMagnitude(1000));     // 3

  // --- Toplu Hesaplama ---
  const { topluHesapla } = LogaritmaKutuphanesi;
  const sonuclar = topluHesapla([1, 2, 4, 8, 16], "log2");
  // [{x:1,sonuc:0}, {x:2,sonuc:1}, ...]

  // --- Logaritmik Regresyon ---
  const { logaritmikRegresyon } = LogaritmaKutuphanesi;
  const xVeri = [1, 2, 4, 8, 16];
  const yVeri = [0, 0.7, 1.4, 2.1, 2.8];
  const { a, b, r2 } = logaritmikRegresyon(xVeri, yVeri);
*/