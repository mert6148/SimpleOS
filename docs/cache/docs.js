/**
 * @fileoverview Docs — Kapsamlı Döküman Yönetim Sistemi (Renderer API)
 * @version      4.0.0
 * @license      MIT
 *
 * Ana işlemle güvenli iletişim için renderer sürecine API sağlar.
 * Döküman yönetimi, önbellek, arama, versiyon takibi ve daha fazlası.
 *
 * Modüller:
 *  1.  Sabitler ve Yapılandırma
 *  2.  Yardımcı Araçlar (Utils)
 *  3.  Olay Yayıncısı (EventEmitter)
 *  4.  Önbellek Katmanı (Cache)
 *  5.  Durum Yönetimi (State)
 *  6.  DOM İşlemleri
 *  7.  Render Motoru
 *  8.  Arama Motoru
 *  9.  Navigasyon
 * 10.  Versiyon Yönetimi
 * 11.  Tema ve Görünüm
 * 12.  Kısayol Tuşları
 * 13.  Ağ Katmanı (Network)
 * 14.  Güvenlik (Sanitizer)
 * 15.  Eklenti Sistemi (Plugin)
 * 16.  İzin Yönetimi
 * 17.  Erişilebilirlik (A11y)
 * 18.  İçe/Dışa Aktarma
 * 19.  Telemetri ve Loglama
 * 20.  Ana Docs API
 */

"use strict";

// =============================================================================
// BÖLÜM 1 — SABİTLER VE YAPILANDIRMA
// =============================================================================

/** Kütüphane meta bilgileri */
const DOCS_META = Object.freeze({
    VERSION: "4.0.0",
    NAME: "Docs",
    DESCRIPTION: "Kapsamlı Döküman Yönetim Sistemi",
    AUTHOR: "SimpleOS Team",
    LICENSE: "MIT",
    BUILD_DATE: new Date().toISOString(),
});

/** DOM seçiciler */
const SELECTORS = Object.freeze({
    CONTAINER: ".docs-container",
    PANEL: ".docs-panel",
    OVERLAY: ".docs-overlay",
    LINK: ".docs-link",
    CLOSE_BTN: ".docs-close",
    SEARCH_INPUT: ".docs-search-input",
    SEARCH_RESULTS: ".docs-search-results",
    NAV_ITEM: ".docs-nav-item",
    CONTENT_AREA: ".docs-content",
    SIDEBAR: ".docs-sidebar",
    BREADCRUMB: ".docs-breadcrumb",
    TOC: ".docs-toc",
    HEADER: ".docs-header",
    FOOTER: ".docs-footer",
    LOADING: ".docs-loading",
    ERROR_BOX: ".docs-error",
    THEME_TOGGLE: ".docs-theme-toggle",
    FONT_INCREASE: ".docs-font-increase",
    FONT_DECREASE: ".docs-font-decrease",
    PRINT_BTN: ".docs-print",
    BOOKMARK_BTN: ".docs-bookmark",
    SHARE_BTN: ".docs-share",
    FEEDBACK_BTN: ".docs-feedback",
    COPY_BTN: ".docs-copy",
    EXPAND_ALL: ".docs-expand-all",
    COLLAPSE_ALL: ".docs-collapse-all",
    SCROLL_TOP: ".docs-scroll-top",
    VERSION_SELECT: ".docs-version-select",
    LANG_SELECT: ".docs-lang-select",
    PROGRESS_BAR: ".docs-progress",
});

/** CSS sınıfları */
const CSS_CLASSES = Object.freeze({
    ACTIVE: "docs-active",
    OPEN: "docs-open",
    CLOSED: "docs-closed",
    LOADING: "docs-loading",
    ERROR: "docs-error",
    SUCCESS: "docs-success",
    WARNING: "docs-warning",
    HIDDEN: "docs-hidden",
    VISIBLE: "docs-visible",
    EXPANDED: "docs-expanded",
    COLLAPSED: "docs-collapsed",
    HIGHLIGHTED: "docs-highlighted",
    BOOKMARKED: "docs-bookmarked",
    DARK_THEME: "docs-theme-dark",
    LIGHT_THEME: "docs-theme-light",
    RTL: "docs-rtl",
    FOCUSED: "docs-focused",
    DRAGGING: "docs-dragging",
    TRANSITIONING: "docs-transitioning",
    PINNED: "docs-pinned",
    STICKY: "docs-sticky",
    TRUNCATED: "docs-truncated",
    INLINE: "docs-inline",
    FULLSCREEN: "docs-fullscreen",
    RESIZING: "docs-resizing",
    SELECTED: "docs-selected",
    DISABLED: "docs-disabled",
    READONLY: "docs-readonly",
});

/** Varsayılan yapılandırma */
const DEFAULT_CONFIG = Object.freeze({
    baseUrl: "/docs",
    apiUrl: "/api/docs",
    locale: "tr",
    theme: "light",
    fontSize: 16,
    fontSizeMin: 12,
    fontSizeMax: 24,
    animationDuration: 250,
    searchDebounce: 300,
    cacheMaxAge: 3600000,       // 1 saat (ms)
    cacheMaxSize: 100,
    networkTimeout: 10000,
    retryCount: 3,
    retryDelay: 1000,
    maxHistorySize: 50,
    maxBookmarks: 200,
    scrollThreshold: 200,
    tocMinHeadings: 3,
    printDelay: 500,
    autoSaveInterval: 30000,
    inactivityTimeout: 600000,        // 10 dakika (ms)
    highlightColor: "#ffeb3b",
    searchResultsLimit: 20,
    lazyLoadThreshold: 0.1,
    maxFileSize: 5 * 1024 * 1024, // 5 MB
    allowedFileTypes: ["md", "html", "txt", "pdf"],
    telemetryEnabled: true,
    debugMode: false,
    hotkeys: true,
    rtl: false,
    readingProgress: true,
    smoothScroll: true,
    tocAutoHighlight: true,
    versionTracking: true,
    offlineMode: false,
    compressionEnabled: true,
    sanitizeContent: true,
    allowExternalLinks: false,
    confirmNavigation: false,
    persistState: true,
    storageKey: "docs_state",
    storageVersion: "4",
});

/** Olay türleri */
const EVENTS = Object.freeze({
    INIT: "docs:init",
    READY: "docs:ready",
    DESTROY: "docs:destroy",
    OPEN: "docs:open",
    CLOSE: "docs:close",
    NAVIGATE: "docs:navigate",
    NAVIGATE_BACK: "docs:navigate:back",
    NAVIGATE_FWD: "docs:navigate:forward",
    SEARCH: "docs:search",
    SEARCH_CLEAR: "docs:search:clear",
    SEARCH_SELECT: "docs:search:select",
    LOAD_START: "docs:load:start",
    LOAD_END: "docs:load:end",
    LOAD_ERROR: "docs:load:error",
    RENDER: "docs:render",
    RENDER_START: "docs:render:start",
    RENDER_END: "docs:render:end",
    CACHE_HIT: "docs:cache:hit",
    CACHE_MISS: "docs:cache:miss",
    CACHE_CLEAR: "docs:cache:clear",
    THEME_CHANGE: "docs:theme:change",
    FONT_CHANGE: "docs:font:change",
    BOOKMARK_ADD: "docs:bookmark:add",
    BOOKMARK_REMOVE: "docs:bookmark:remove",
    PLUGIN_LOAD: "docs:plugin:load",
    PLUGIN_UNLOAD: "docs:plugin:unload",
    ERROR: "docs:error",
    WARNING: "docs:warning",
    SCROLL: "docs:scroll",
    RESIZE: "docs:resize",
    FULLSCREEN: "docs:fullscreen",
    PRINT: "docs:print",
    COPY: "docs:copy",
    SHARE: "docs:share",
    FEEDBACK: "docs:feedback",
    VERSION_CHANGE: "docs:version:change",
    LANG_CHANGE: "docs:lang:change",
    OFFLINE: "docs:offline",
    ONLINE: "docs:online",
    STATE_SAVE: "docs:state:save",
    STATE_RESTORE: "docs:state:restore",
    HOTKEY: "docs:hotkey",
    FOCUS: "docs:focus",
    BLUR: "docs:blur",
    TOC_CLICK: "docs:toc:click",
    HEADING_VISIBLE: "docs:heading:visible",
    CODE_COPY: "docs:code:copy",
    LINK_EXTERNAL: "docs:link:external",
    LINK_ANCHOR: "docs:link:anchor",
    FEEDBACK_SUBMIT: "docs:feedback:submit",
});

/** HTTP durum kodları */
const HTTP_STATUS = Object.freeze({
    OK: 200,
    CREATED: 201,
    NO_CONTENT: 204,
    NOT_MODIFIED: 304,
    BAD_REQUEST: 400,
    UNAUTHORIZED: 401,
    FORBIDDEN: 403,
    NOT_FOUND: 404,
    CONFLICT: 409,
    TOO_MANY: 429,
    SERVER_ERROR: 500,
    SERVICE_UNAVAIL: 503,
});

/** Hata türleri */
const ERROR_TYPES = Object.freeze({
    NETWORK: "NetworkError",
    PARSE: "ParseError",
    NOT_FOUND: "NotFoundError",
    TIMEOUT: "TimeoutError",
    PERMISSION: "PermissionError",
    VALIDATION: "ValidationError",
    CACHE: "CacheError",
    RENDER: "RenderError",
    PLUGIN: "PluginError",
    STORAGE: "StorageError",
    UNKNOWN: "UnknownError",
});

/** Log seviyeleri */
const LOG_LEVELS = Object.freeze({
    DEBUG: 0,
    INFO: 1,
    WARN: 2,
    ERROR: 3,
    SILENT: 4,
});

// =============================================================================
// BÖLÜM 2 — YARDIMCI ARAÇLAR (UTILS)
// =============================================================================

const Utils = (() => {
    /**
     * Debounce — fonksiyon çağrısını geciktirir
     * @param {Function} fn
     * @param {number}   gecikme — ms
     * @returns {Function}
     */
    function debounce(fn, gecikme) {
        let zamanlayici;
        function iptalEt() { clearTimeout(zamanlayici); }
        function geciktir(...args) {
            iptalEt();
            zamanlayici = setTimeout(() => fn.apply(this, args), gecikme);
        }
        geciktir.iptalEt = iptalEt;
        return geciktir;
    }

    /**
     * Throttle — fonksiyon çağrısını sınırlar
     * @param {Function} fn
     * @param {number}   aralik — ms
     * @returns {Function}
     */
    function throttle(fn, aralik) {
        let sonCagri = 0;
        return function (...args) {
            const simdi = Date.now();
            if (simdi - sonCagri >= aralik) {
                sonCagri = simdi;
                return fn.apply(this, args);
            }
        };
    }

    /**
     * Derin kopya (deep clone)
     * @param {*} nesne
     * @returns {*}
     */
    function derinKopya(nesne) {
        if (nesne === null || typeof nesne !== "object") return nesne;
        if (nesne instanceof Date) return new Date(nesne.valueOf());
        if (nesne instanceof RegExp) return new RegExp(nesne);
        if (nesne instanceof Map) return new Map([...nesne].map(([k, v]) => [k, derinKopya(v)]));
        if (nesne instanceof Set) return new Set([...nesne].map(derinKopya));
        if (Array.isArray(nesne)) return nesne.map(derinKopya);
        const kopya = Object.create(Object.getPrototypeOf(nesne));
        for (const [anahtar, deger] of Object.entries(nesne)) {
            kopya[anahtar] = derinKopya(deger);
        }
        return kopya;
    }

    /**
     * Nesneleri derin birleştirme (deep merge)
     * @param {...Object} nesneler
     * @returns {Object}
     */
    function derinBirlestir(...nesneler) {
        const sonuc = {};
        for (const nesne of nesneler) {
            if (!nesne || typeof nesne !== "object") continue;
            for (const [anahtar, deger] of Object.entries(nesne)) {
                if (deger && typeof deger === "object" && !Array.isArray(deger)) {
                    sonuc[anahtar] = derinBirlestir(sonuc[anahtar] || {}, deger);
                } else {
                    sonuc[anahtar] = deger;
                }
            }
        }
        return sonuc;
    }

    /**
     * URL doğrulama
     * @param {string} url
     * @returns {boolean}
     */
    function gecerliURL(url) {
        try {
            new URL(url, window.location.origin);
            return true;
        } catch {
            return false;
        }
    }

    /**
     * URL birleştirme — base + path
     * @param {string} base
     * @param {string} path
     * @returns {string}
     */
    function urlBirlestir(base, path) {
        const temizBase = base.replace(/\/+$/, "");
        const temizPath = path.replace(/^\/+/, "");
        return `${temizBase}/${temizPath}`;
    }

    /**
     * Query string nesneye dönüştürme
     * @param {string} query
     * @returns {Object}
     */
    function queryParse(query) {
        return Object.fromEntries(
            new URLSearchParams(query).entries()
        );
    }

    /**
     * Nesneyi query string'e dönüştürme
     * @param {Object} nesne
     * @returns {string}
     */
    function queryStringify(nesne) {
        return new URLSearchParams(
            Object.entries(nesne).filter(([, v]) => v !== null && v !== undefined)
        ).toString();
    }

    /**
     * HTML özel karakterlerini kaçış
     * @param {string} metin
     * @returns {string}
     */
    function htmlKacis(metin) {
        const karakter = { "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" };
        return String(metin).replace(/[&<>"']/g, c => karakter[c]);
    }

    /**
     * HTML kaçış geri alma
     * @param {string} metin
     * @returns {string}
     */
    function htmlGeriAl(metin) {
        const tanim = { "&amp;": "&", "&lt;": "<", "&gt;": ">", "&quot;": '"', "&#39;": "'" };
        return String(metin).replace(/&amp;|&lt;|&gt;|&quot;|&#39;/g, e => tanim[e]);
    }

    /**
     * Slug oluşturma — URL dostu metin
     * @param {string} metin
     * @returns {string}
     */
    function slugOlustur(metin) {
        return metin
            .toLowerCase()
            .normalize("NFD")
            .replace(/[\u0300-\u036f]/g, "")
            .replace(/[çÇ]/g, "c")
            .replace(/[şŞ]/g, "s")
            .replace(/[ğĞ]/g, "g")
            .replace(/[üÜ]/g, "u")
            .replace(/[öÖ]/g, "o")
            .replace(/[ıİ]/g, "i")
            .replace(/[^a-z0-9\s-]/g, "")
            .replace(/\s+/g, "-")
            .replace(/-+/g, "-")
            .trim()
            .replace(/^-|-$/g, "");
    }

    /**
     * Benzersiz ID üretimi
     * @param {string} [ön] — önek
     * @returns {string}
     */
    function benzersizID(on = "docs") {
        return `${on}-${Date.now().toString(36)}-${Math.random().toString(36).slice(2, 9)}`;
    }

    /**
     * Milisaniye bekleme (Promise tabanlı)
     * @param {number} ms
     * @returns {Promise<void>}
     */
    function bekle(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }

    /**
     * Dosya boyutunu okunabilir stringe çevir
     * @param {number} bayt
     * @returns {string}
     */
    function dosyaBoyutu(bayt) {
        if (bayt < 1024) return `${bayt} B`;
        if (bayt < 1048576) return `${(bayt / 1024).toFixed(1)} KB`;
        if (bayt < 1073741824) return `${(bayt / 1048576).toFixed(1)} MB`;
        return `${(bayt / 1073741824).toFixed(1)} GB`;
    }

    /**
     * Tarihi okunabilir formata çevir
     * @param {Date|string|number} tarih
     * @param {string}             locale
     * @returns {string}
     */
    function tarihFormatla(tarih, locale = "tr-TR") {
        try {
            return new Intl.DateTimeFormat(locale, {
                year: "numeric", month: "long", day: "numeric",
                hour: "2-digit", minute: "2-digit"
            }).format(new Date(tarih));
        } catch {
            return String(tarih);
        }
    }

    /**
     * Okuma süresi tahmini (kelime sayısına göre)
     * @param {string} metin
     * @returns {string}
     */
    function okumaSuresi(metin) {
        const kelimeSayisi = metin.trim().split(/\s+/).length;
        const dakika = Math.ceil(kelimeSayisi / 200);
        return dakika === 1 ? "1 dakika okuma" : `${dakika} dakika okuma`;
    }

    /**
     * Metni kırp (truncate)
     * @param {string} metin
     * @param {number} uzunluk
     * @param {string} son
     * @returns {string}
     */
    function metinKirp(metin, uzunluk = 100, son = "…") {
        if (metin.length <= uzunluk) return metin;
        return metin.slice(0, uzunluk - son.length) + son;
    }

    /**
     * Metinde kelime vurgulama — arama için
     * @param {string} metin
     * @param {string} aranan
     * @returns {string} HTML ile vurgulanmış metin
     */
    function kelimeVurgula(metin, aranan) {
        if (!aranan) return htmlKacis(metin);
        const temizAranan = htmlKacis(aranan).replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
        return htmlKacis(metin).replace(
            new RegExp(`(${temizAranan})`, "gi"),
            '<mark class="docs-highlight">$1</mark>'
        );
    }

    /**
     * LocalStorage wrapper — JSON destekli
     */
    const depolama = {
        oku(anahtar, varsayilan = null) {
            try {
                const deger = localStorage.getItem(anahtar);
                return deger !== null ? JSON.parse(deger) : varsayilan;
            } catch {
                return varsayilan;
            }
        },
        yaz(anahtar, deger) {
            try {
                localStorage.setItem(anahtar, JSON.stringify(deger));
                return true;
            } catch (e) {
                console.warn("Docs: Depolama hatası →", e.message);
                return false;
            }
        },
        sil(anahtar) {
            try { localStorage.removeItem(anahtar); return true; }
            catch { return false; }
        },
        temizle(onekli) {
            if (!onekli) { localStorage.clear(); return; }
            Object.keys(localStorage)
                .filter(k => k.startsWith(onekli))
                .forEach(k => localStorage.removeItem(k));
        },
        boyut() {
            let toplam = 0;
            for (const anahtar of Object.keys(localStorage)) {
                toplam += (localStorage[anahtar].length + anahtar.length) * 2;
            }
            return toplam;
        },
    };

    /**
     * Kopyalama (clipboard) — modern API ile
     * @param {string} metin
     * @returns {Promise<boolean>}
     */
    async function panoyaKopyala(metin) {
        try {
            if (navigator.clipboard?.writeText) {
                await navigator.clipboard.writeText(metin);
                return true;
            }
            // Fallback
            const textarea = document.createElement("textarea");
            textarea.value = metin;
            textarea.style.cssText = "position:fixed;top:-9999px;left:-9999px;opacity:0";
            document.body.appendChild(textarea);
            textarea.select();
            const basarili = document.execCommand("copy");
            document.body.removeChild(textarea);
            return basarili;
        } catch {
            return false;
        }
    }

    /**
     * requestAnimationFrame Polyfill
     */
    const raf = window.requestAnimationFrame || (fn => setTimeout(fn, 16));
    const craf = window.cancelAnimationFrame || clearTimeout;

    /**
     * Elementin görünür alanda olup olmadığı
     * @param {Element} el
     * @returns {boolean}
     */
    function gorunurAlanda(el) {
        const rect = el.getBoundingClientRect();
        return (
            rect.top >= 0 &&
            rect.left >= 0 &&
            rect.bottom <= (window.innerHeight || document.documentElement.clientHeight) &&
            rect.right <= (window.innerWidth || document.documentElement.clientWidth)
        );
    }

    /**
     * Sayfa scroll pozisyonu
     * @returns {{x: number, y: number}}
     */
    function scrollPozisyon() {
        return {
            x: window.pageXOffset || document.documentElement.scrollLeft,
            y: window.pageYOffset || document.documentElement.scrollTop,
        };
    }

    /**
     * Yumuşak scroll (smooth scroll)
     * @param {Element|number} hedef — element veya Y pozisyonu
     * @param {number}         ust   — üst boşluk (px)
     */
    function yumusak_scroll(hedef, ust = 80) {
        const y = typeof hedef === "number"
            ? hedef
            : hedef.getBoundingClientRect().top + window.pageYOffset - ust;
        window.scrollTo({ top: y, behavior: "smooth" });
    }

    /**
     * Dil desteği — RTL kontrolü
     * @param {string} dil
     * @returns {boolean}
     */
    function rtlDilMi(dil) {
        const rtlDiller = ["ar", "he", "fa", "ur", "ps", "sd", "yi"];
        return rtlDiller.includes(dil.split("-")[0].toLowerCase());
    }

    /**
     * İçeriğin hash'ini hesapla (basit)
     * @param {string} metin
     * @returns {string}
     */
    function basitHash(metin) {
        let hash = 0;
        for (let i = 0; i < metin.length; i++) {
            const kod = metin.charCodeAt(i);
            hash = ((hash << 5) - hash) + kod;
            hash |= 0;
        }
        return Math.abs(hash).toString(36);
    }

    /**
     * MutationObserver kolaylaştırıcı
     * @param {Element}  hedef
     * @param {Function} geri_donus
     * @param {Object}   [secenekler]
     * @returns {MutationObserver}
     */
    function gozlemle(hedef, geri_donus, secenekler = { childList: true, subtree: true }) {
        const gozlemci = new MutationObserver(geri_donus);
        gozlemci.observe(hedef, secenekler);
        return gozlemci;
    }

    /**
     * IntersectionObserver kolaylaştırıcı
     * @param {Element}  hedef
     * @param {Function} geri_donus
     * @param {Object}   [secenekler]
     * @returns {IntersectionObserver}
     */
    function kesisme_gozlemle(hedef, geri_donus, secenekler = {}) {
        const gozlemci = new IntersectionObserver(geri_donus, secenekler);
        gozlemci.observe(hedef);
        return gozlemci;
    }

    /**
     * ResizeObserver kolaylaştırıcı
     * @param {Element}  hedef
     * @param {Function} geri_donus
     * @returns {ResizeObserver}
     */
    function boyut_gozlemle(hedef, geri_donus) {
        const gozlemci = new ResizeObserver(geri_donus);
        gozlemci.observe(hedef);
        return gozlemci;
    }

    /**
     * Sayı biçimlendirme
     * @param {number} sayi
     * @param {string} locale
     * @returns {string}
     */
    function sayiBicimle(sayi, locale = "tr-TR") {
        return new Intl.NumberFormat(locale).format(sayi);
    }

    /**
     * İçerik metin ayıklama — HTML'den düz metin
     * @param {string} html
     * @returns {string}
     */
    function htmldenMetin(html) {
        const div = document.createElement("div");
        div.innerHTML = html;
        return div.textContent || div.innerText || "";
    }

    /**
     * Veriyi base64 kodlama
     * @param {string} metin
     * @returns {string}
     */
    function base64Kodla(metin) {
        try {
            return btoa(encodeURIComponent(metin));
        } catch {
            return btoa(metin);
        }
    }

    /**
     * Base64 çözme
     * @param {string} kodlu
     * @returns {string}
     */
    function base64Coz(kodlu) {
        try {
            return decodeURIComponent(atob(kodlu));
        } catch {
            return atob(kodlu);
        }
    }

    /**
     * Cihaz tipini algıla
     * @returns {"mobile"|"tablet"|"desktop"}
     */
    function cihazTipi() {
        const genislik = window.innerWidth;
        if (genislik < 768) return "mobile";
        if (genislik < 1024) return "tablet";
        return "desktop";
    }

    /**
     * Renk parlaklığını hesapla (WCAG için)
     * @param {string} renk — hex renk (#RRGGBB)
     * @returns {number} 0-255 arası
     */
    function renkParlakligi(renk) {
        const r = parseInt(renk.slice(1, 3), 16);
        const g = parseInt(renk.slice(3, 5), 16);
        const b = parseInt(renk.slice(5, 7), 16);
        return 0.299 * r + 0.587 * g + 0.114 * b;
    }

    /**
     * JSON şemasını doğrula (basit)
     * @param {Object} veri
     * @param {Object} sema
     * @returns {{gecerli: boolean, hatalar: string[]}}
     */
    function semaDogrula(veri, sema) {
        const hatalar = [];
        for (const [alan, kural] of Object.entries(sema)) {
            const deger = veri[alan];
            if (kural.zorunlu && (deger === undefined || deger === null)) {
                hatalar.push(`${alan} alanı zorunludur.`);
                continue;
            }
            if (deger !== undefined && kural.tip && typeof deger !== kural.tip) {
                hatalar.push(`${alan} tipi ${kural.tip} olmalıdır, ${typeof deger} bulundu.`);
            }
            if (typeof deger === "string") {
                if (kural.minLength && deger.length < kural.minLength) {
                    hatalar.push(`${alan} en az ${kural.minLength} karakter olmalıdır.`);
                }
                if (kural.maxLength && deger.length > kural.maxLength) {
                    hatalar.push(`${alan} en fazla ${kural.maxLength} karakter olabilir.`);
                }
                if (kural.desen && !kural.desen.test(deger)) {
                    hatalar.push(`${alan} geçersiz format.`);
                }
            }
            if (typeof deger === "number") {
                if (kural.min !== undefined && deger < kural.min) {
                    hatalar.push(`${alan} en az ${kural.min} olmalıdır.`);
                }
                if (kural.max !== undefined && deger > kural.max) {
                    hatalar.push(`${alan} en fazla ${kural.max} olabilir.`);
                }
            }
        }
        return { gecerli: hatalar.length === 0, hatalar };
    }

    return Object.freeze({
        debounce, throttle, derinKopya, derinBirlestir,
        gecerliURL, urlBirlestir, queryParse, queryStringify,
        htmlKacis, htmlGeriAl, slugOlustur, benzersizID,
        bekle, dosyaBoyutu, tarihFormatla, okumaSuresi,
        metinKirp, kelimeVurgula, depolama, panoyaKopyala,
        raf, craf, gorunurAlanda, scrollPozisyon, yumusak_scroll,
        rtlDilMi, basitHash, gozlemle, kesisme_gozlemle, boyut_gozlemle,
        sayiBicimle, htmldenMetin, base64Kodla, base64Coz,
        cihazTipi, renkParlakligi, semaDogrula,
    });
})();

// =============================================================================
// BÖLÜM 3 — OLAY YAYINCISI (EventEmitter)
// =============================================================================

class DocsEventEmitter {
    constructor() {
        /** @type {Map<string, Set<Function>>} */
        this._dinleyiciler = new Map();
        /** @type {Map<string, Set<Function>>} */
        this._tekSeferlik = new Map();
        /** @type {Array<{olay: string, veri: *, zaman: number}>} */
        this._gecmis = [];
        this._gecmisMax = 500;
        this._etkin = true;
        this._filtrelenenler = new Set();
    }

    /**
     * Olay dinleyicisi ekle
     * @param {string}   olay
     * @param {Function} isleyici
     * @returns {this}
     */
    on(olay, isleyici) {
        if (!this._dinleyiciler.has(olay)) {
            this._dinleyiciler.set(olay, new Set());
        }
        this._dinleyiciler.get(olay).add(isleyici);
        return this;
    }

    /**
     * Tek seferlik olay dinleyicisi
     * @param {string}   olay
     * @param {Function} isleyici
     * @returns {this}
     */
    once(olay, isleyici) {
        if (!this._tekSeferlik.has(olay)) {
            this._tekSeferlik.set(olay, new Set());
        }
        this._tekSeferlik.get(olay).add(isleyici);
        return this;
    }

    /**
     * Olay dinleyicisini kaldır
     * @param {string}   olay
     * @param {Function} [isleyici] — belirtilmezse tüm dinleyiciler kaldırılır
     * @returns {this}
     */
    off(olay, isleyici) {
        if (!isleyici) {
            this._dinleyiciler.delete(olay);
            this._tekSeferlik.delete(olay);
        } else {
            this._dinleyiciler.get(olay)?.delete(isleyici);
            this._tekSeferlik.get(olay)?.delete(isleyici);
        }
        return this;
    }

    /**
     * Olay yayınla
     * @param {string} olay
     * @param {*}      [veri]
     * @returns {boolean} — en az bir dinleyici tetiklendiyse true
     */
    emit(olay, veri) {
        if (!this._etkin || this._filtrelenenler.has(olay)) return false;

        // Geçmişe kaydet
        this._gecmis.push({ olay, veri, zaman: Date.now() });
        if (this._gecmis.length > this._gecmisMax) {
            this._gecmis.shift();
        }

        let tetiklendi = false;

        // Kalıcı dinleyiciler
        const daimi = this._dinleyiciler.get(olay);
        if (daimi?.size) {
            for (const fn of daimi) {
                try { fn(veri); tetiklendi = true; }
                catch (e) { console.error(`Docs: Olay hatası [${olay}] →`, e); }
            }
        }

        // Tek seferlik dinleyiciler
        const tekSef = this._tekSeferlik.get(olay);
        if (tekSef?.size) {
            for (const fn of tekSef) {
                try { fn(veri); tetiklendi = true; }
                catch (e) { console.error(`Docs: Tek seferlik olay hatası [${olay}] →`, e); }
            }
            this._tekSeferlik.delete(olay);
        }

        // Wildcard (*) dinleyiciler
        const wildcard = this._dinleyiciler.get("*");
        if (wildcard?.size) {
            for (const fn of wildcard) {
                try { fn(olay, veri); tetiklendi = true; }
                catch (e) { console.error("Docs: Wildcard dinleyici hatası →", e); }
            }
        }

        return tetiklendi;
    }

    /**
     * Olay için Promise döner — tek seferlik bekleme
     * @param {string} olay
     * @param {number} [zaman_asimi] — ms, 0 = sınırsız
     * @returns {Promise<*>}
     */
    bekle(olay, zaman_asimi = 0) {
        return new Promise((coz, reddet) => {
            let zamanlayici;
            const isleyici = (veri) => {
                if (zamanlayici) clearTimeout(zamanlayici);
                coz(veri);
            };
            this.once(olay, isleyici);
            if (zaman_asimi > 0) {
                zamanlayici = setTimeout(() => {
                    this.off(olay, isleyici);
                    reddet(new Error(`Zaman aşımı: ${olay} olayı ${zaman_asimi}ms içinde tetiklenmedi.`));
                }, zaman_asimi);
            }
        });
    }

    /** Tüm dinleyicileri temizle */
    temizle() {
        this._dinleyiciler.clear();
        this._tekSeferlik.clear();
        return this;
    }

    /**
     * Olay sayısını döndür
     * @param {string} [olay]
     * @returns {number}
     */
    sayac(olay) {
        if (olay) {
            return (this._dinleyiciler.get(olay)?.size || 0) +
                (this._tekSeferlik.get(olay)?.size || 0);
        }
        let toplam = 0;
        for (const set of this._dinleyiciler.values()) toplam += set.size;
        for (const set of this._tekSeferlik.values()) toplam += set.size;
        return toplam;
    }

    /**
     * Kayıtlı olayları listele
     * @returns {string[]}
     */
    olaylar() {
        return [...new Set([
            ...this._dinleyiciler.keys(),
            ...this._tekSeferlik.keys(),
        ])];
    }

    /**
     * Olay geçmişini döndür
     * @param {string} [olay] — filtre
     * @returns {Array}
     */
    gecmis(olay) {
        if (olay) return this._gecmis.filter(g => g.olay === olay);
        return [...this._gecmis];
    }

    /** Yayını durdur */
    durdur() { this._etkin = false; return this; }

    /** Yayını devam ettir */
    devamEt() { this._etkin = true; return this; }

    /**
     * Belirli olayları filtrele (yayımlanmasın)
     * @param {...string} olaylar
     */
    filtrele(...olaylar) {
        for (const o of olaylar) this._filtrelenenler.add(o);
        return this;
    }

    /**
     * Filtreyi kaldır
     * @param {...string} olaylar
     */
    filtreKaldir(...olaylar) {
        for (const o of olaylar) this._filtrelenenler.delete(o);
        return this;
    }
}

// =============================================================================
// BÖLÜM 4 — ÖNBELLEK KATMANI (Cache)
// =============================================================================

class DocsCache {
    /**
     * @param {Object} [secenekler]
     * @param {number} secenekler.maxBoyut   — maksimum giriş sayısı
     * @param {number} secenekler.maxYas     — milisaniye cinsinden TTL
     * @param {string} secenekler.isim       — önbellek ismi (loglama için)
     */
    constructor(secenekler = {}) {
        this._maxBoyut = secenekler.maxBoyut ?? DEFAULT_CONFIG.cacheMaxSize;
        this._maxYas = secenekler.maxYas ?? DEFAULT_CONFIG.cacheMaxAge;
        this._isim = secenekler.isim ?? "DocsCache";

        /** @type {Map<string, {deger: *, zaman: number, erismeSayisi: number, boyut: number}>} */
        this._depo = new Map();

        /** İstatistikler */
        this._istatistik = {
            isabet: 0,
            iskalamak: 0,
            yazma: 0,
            silme: 0,
            temizleme: 0,
        };
    }

    /**
     * Değer oku
     * @param {string} anahtar
     * @returns {* | undefined}
     */
    get(anahtar) {
        const giris = this._depo.get(anahtar);
        if (!giris) { this._istatistik.iskalamak++; return undefined; }
        if (this._suresiGectimi(giris)) {
            this._depo.delete(anahtar);
            this._istatistik.iskalamak++;
            return undefined;
        }
        giris.erismeSayisi++;
        this._istatistik.isabet++;
        return giris.deger;
    }

    /**
     * Değer yaz
     * @param {string} anahtar
     * @param {*}      deger
     * @param {number} [ttl] — özel TTL (ms)
     */
    set(anahtar, deger, ttl) {
        // Kapasite aşımında LRU temizleme
        if (this._depo.size >= this._maxBoyut && !this._depo.has(anahtar)) {
            this._lruTemizle();
        }
        const boyut = this._boyutHesapla(deger);
        this._depo.set(anahtar, {
            deger,
            zaman: Date.now(),
            ttl: ttl ?? this._maxYas,
            erismeSayisi: 0,
            boyut,
        });
        this._istatistik.yazma++;
        return this;
    }

    /**
     * Değer var mı kontrol et
     * @param {string} anahtar
     * @returns {boolean}
     */
    has(anahtar) {
        const giris = this._depo.get(anahtar);
        if (!giris) return false;
        if (this._suresiGectimi(giris)) {
            this._depo.delete(anahtar);
            return false;
        }
        return true;
    }

    /**
     * Değer sil
     * @param {string} anahtar
     * @returns {boolean}
     */
    delete(anahtar) {
        const silindi = this._depo.delete(anahtar);
        if (silindi) this._istatistik.silme++;
        return silindi;
    }

    /** Tüm önbelleği temizle */
    clear() {
        this._depo.clear();
        this._istatistik.temizleme++;
        return this;
    }

    /**
     * Süresi geçmiş girdileri temizle
     * @returns {number} temizlenen giriş sayısı
     */
    temizleSuresiGecmis() {
        let sayac = 0;
        const simdi = Date.now();
        for (const [anahtar, giris] of this._depo) {
            if (simdi - giris.zaman > (giris.ttl ?? this._maxYas)) {
                this._depo.delete(anahtar);
                sayac++;
            }
        }
        return sayac;
    }

    /**
     * Cache-or-fetch — varsa önbellekten, yoksa üreticiden al
     * @param {string}   anahtar
     * @param {Function} uretici — async () => değer
     * @param {number}   [ttl]
     * @returns {Promise<*>}
     */
    async cacheOrFetch(anahtar, uretici, ttl) {
        if (this.has(anahtar)) return this.get(anahtar);
        const deger = await uretici();
        this.set(anahtar, deger, ttl);
        return deger;
    }

    /** İstatistikleri döndür */
    istatistik() {
        const toplam = this._istatistik.isabet + this._istatistik.iskalamak;
        return {
            ...this._istatistik,
            boyut: this._depo.size,
            isabetorani: toplam > 0 ? (this._istatistik.isabet / toplam * 100).toFixed(1) + "%" : "—",
        };
    }

    /** LRU — en az erişilen giriş sil */
    _lruTemizle() {
        let enAz = Infinity;
        let enAzAnahtar;
        for (const [anahtar, giris] of this._depo) {
            if (giris.erismeSayisi < enAz) {
                enAz = giris.erismeSayisi;
                enAzAnahtar = anahtar;
            }
        }
        if (enAzAnahtar) this._depo.delete(enAzAnahtar);
    }

    /** TTL kontrolü */
    _suresiGectimi(giris) {
        return Date.now() - giris.zaman > (giris.ttl ?? this._maxYas);
    }

    /** Yaklaşık boyut hesabı (bayt) */
    _boyutHesapla(deger) {
        try {
            return JSON.stringify(deger).length * 2;
        } catch {
            return 0;
        }
    }

    /** Toplam önbellek boyutu (bayt) */
    toplamBoyut() {
        let toplam = 0;
        for (const giris of this._depo.values()) toplam += giris.boyut;
        return toplam;
    }

    /** Önbellek içeriğini listele */
    anahtarlar() {
        return [...this._depo.keys()];
    }

    /** Belirli önek ile anahtarları sil */
    onekSil(onek) {
        let sayac = 0;
        for (const anahtar of this._depo.keys()) {
            if (anahtar.startsWith(onek)) {
                this._depo.delete(anahtar);
                sayac++;
            }
        }
        return sayac;
    }

    /** Önbelleği JSON olarak serileştir */
    serialize() {
        const cikti = {};
        for (const [anahtar, giris] of this._depo) {
            if (!this._suresiGectimi(giris)) {
                cikti[anahtar] = giris;
            }
        }
        return JSON.stringify(cikti);
    }

    /** JSON'dan önbelleği yükle */
    deserialize(json) {
        try {
            const veri = JSON.parse(json);
            for (const [anahtar, giris] of Object.entries(veri)) {
                if (!this._suresiGectimi(giris)) {
                    this._depo.set(anahtar, giris);
                }
            }
        } catch (e) {
            console.warn("DocsCache: Seri çözme hatası →", e.message);
        }
        return this;
    }
}

// =============================================================================
// BÖLÜM 5 — DURUM YÖNETİMİ (State)
// =============================================================================

class DocsState {
    /**
     * @param {Object} baslangic — başlangıç durumu
     * @param {Object} [secenekler]
     */
    constructor(baslangic = {}, secenekler = {}) {
        this._durum = { ...baslangic };
        this._onceki = null;
        this._gecmis = [];
        this._gelecek = [];
        this._maxGecmis = secenekler.maxGecmis ?? 50;
        this._emitter = new DocsEventEmitter();
        this._abonelikler = new Map();
        this._donmusAlanlar = new Set(secenekler.donmusAlanlar ?? []);
    }

    /**
     * Değer oku
     * @param {string} [yol] — nokta notasyonu: "kullanici.isim"
     * @returns {*}
     */
    get(yol) {
        if (!yol) return { ...this._durum };
        const parcalar = yol.split(".");
        let deger = this._durum;
        for (const parça of parcalar) {
            if (deger == null) return undefined;
            deger = deger[parça];
        }
        return deger;
    }

    /**
     * Durumu güncelle
     * @param {Object|Function} guncelleme — nesne veya (mevcutDurum) => yeniDurum
     * @param {string} [aciklama] — geçmiş için açıklama
     * @returns {this}
     */
    set(guncelleme, aciklama = "") {
        const yeniDurum = typeof guncelleme === "function"
            ? guncelleme(this._durum)
            : guncelleme;

        // Dondurulmuş alan kontrolü
        for (const alan of this._donmusAlanlar) {
            if (alan in yeniDurum && yeniDurum[alan] !== this._durum[alan]) {
                console.warn(`DocsState: "${alan}" alanı dondurulmuştur.`);
                delete yeniDurum[alan];
            }
        }

        // Geçmişi kaydet
        this._gecmis.push({
            durum: { ...this._durum },
            aciklama,
            zaman: Date.now(),
        });
        if (this._gecmis.length > this._maxGecmis) this._gecmis.shift();
        this._gelecek = []; // Geri alındıktan sonra yeni değişiklik

        this._onceki = { ...this._durum };
        this._durum = { ...this._durum, ...yeniDurum };

        // Değişen alanlar için bildirim
        for (const [alan, deger] of Object.entries(yeniDurum)) {
            if (deger !== this._onceki[alan]) {
                this._emitter.emit(`degisim:${alan}`, { yeni: deger, eski: this._onceki[alan] });
                this._abonelik_tetikle(alan, deger, this._onceki[alan]);
            }
        }
        this._emitter.emit("degisim", { yeni: this._durum, eski: this._onceki });

        return this;
    }

    /**
     * Geri al
     * @returns {boolean}
     */
    geriAl() {
        if (this._gecmis.length === 0) return false;
        const onceki = this._gecmis.pop();
        this._gelecek.push({ durum: { ...this._durum }, zaman: Date.now() });
        this._durum = { ...onceki.durum };
        this._emitter.emit("geri_al", this._durum);
        return true;
    }

    /**
     * Yinele
     * @returns {boolean}
     */
    yinele() {
        if (this._gelecek.length === 0) return false;
        const ileri = this._gelecek.pop();
        this._gecmis.push({ durum: { ...this._durum }, zaman: Date.now() });
        this._durum = { ...ileri.durum };
        this._emitter.emit("yinele", this._durum);
        return true;
    }

    /**
     * Alan aboneliği
     * @param {string}   alan
     * @param {Function} geri_donus
     * @returns {Function} aboneliği iptal eden fonksiyon
     */
    abone(alan, geri_donus) {
        if (!this._abonelikler.has(alan)) {
            this._abonelikler.set(alan, new Set());
        }
        this._abonelikler.get(alan).add(geri_donus);
        return () => this._abonelikler.get(alan)?.delete(geri_donus);
    }

    /** İç abonelik tetikleyici */
    _abonelik_tetikle(alan, yeni, eski) {
        const aboneler = this._abonelikler.get(alan);
        if (!aboneler) return;
        for (const fn of aboneler) {
            try { fn(yeni, eski); }
            catch (e) { console.error(`DocsState: Abonelik hatası [${alan}] →`, e); }
        }
    }

    /** Durumu sıfırla */
    sifirla(baslangic = {}) {
        this._durum = { ...baslangic };
        this._gecmis = [];
        this._gelecek = [];
        this._emitter.emit("sifirla", this._durum);
        return this;
    }

    /** Geçmişi döndür */
    gecmis() { return [...this._gecmis]; }

    /** EventEmitter'a erişim */
    get olaylar() { return this._emitter; }

    /** Durumu JSON olarak al */
    toJSON() { return JSON.stringify(this._durum); }

    /** JSON'dan yükle */
    fromJSON(json) {
        try {
            this.set(JSON.parse(json), "JSON yükleme");
        } catch (e) {
            console.error("DocsState: JSON yükleme hatası →", e);
        }
        return this;
    }
}

// =============================================================================
// BÖLÜM 6 — DOM İŞLEMLERİ
// =============================================================================

const DOM = (() => {
    /**
     * Güvenli querySelector
     * @param {string}           secici
     * @param {Element|Document} [kapsam]
     * @returns {Element|null}
     */
    function sec(secici, kapsam = document) {
        try { return kapsam.querySelector(secici); }
        catch { return null; }
    }

    /**
     * Güvenli querySelectorAll
     * @param {string}           secici
     * @param {Element|Document} [kapsam]
     * @returns {Element[]}
     */
    function secHepsini(secici, kapsam = document) {
        try { return [...kapsam.querySelectorAll(secici)]; }
        catch { return []; }
    }

    /**
     * Element oluştur
     * @param {string} etiket
     * @param {Object} [ozellikler] — { class, id, ... }
     * @param {...(string|Element)} cocuklar
     * @returns {Element}
     */
    function olustur(etiket, ozellikler = {}, ...cocuklar) {
        const el = document.createElement(etiket);
        for (const [anahtar, deger] of Object.entries(ozellikler)) {
            if (anahtar === "class") {
                el.className = deger;
            } else if (anahtar === "style" && typeof deger === "object") {
                Object.assign(el.style, deger);
            } else if (anahtar.startsWith("data-")) {
                el.setAttribute(anahtar, deger);
            } else if (anahtar === "html") {
                // Güvenli HTML ataması kaldırıldı — sanitize edilmiş içerik için kullan
                el.innerHTML = deger;
            } else if (anahtar in el) {
                el[anahtar] = deger;
            } else {
                el.setAttribute(anahtar, deger);
            }
        }
        for (const cocuk of cocuklar) {
            if (typeof cocuk === "string") {
                el.appendChild(document.createTextNode(cocuk));
            } else if (cocuk instanceof Element) {
                el.appendChild(cocuk);
            }
        }
        return el;
    }

    /**
     * Sınıf ekle/kaldır/değiştir
     */
    const sinif = {
        ekle: (el, ...siniflar) => el?.classList.add(...siniflar),
        kaldir: (el, ...siniflar) => el?.classList.remove(...siniflar),
        degistir(el, ...siniflar) { for (const s of siniflar) el?.classList.toggle(s); },
        var: (el, sinif) => el?.classList.contains(sinif) ?? false,
        ayarla: (el, sinif, aktif) => {
            if (aktif) el?.classList.add(sinif);
            else el?.classList.remove(sinif);
        },
    };

    /**
     * CSS değişkeni ayarla
     * @param {string} isim    — --değişken-adı
     * @param {string} deger
     * @param {Element} [kapsam]
     */
    function cssVar(isim, deger, kapsam = document.documentElement) {
        kapsam?.style.setProperty(isim, deger);
    }

    /**
     * CSS değişkeni oku
     * @param {string}  isim
     * @param {Element} [kapsam]
     * @returns {string}
     */
    function cssVarOku(isim, kapsam = document.documentElement) {
        return getComputedStyle(kapsam).getPropertyValue(isim).trim();
    }

    /**
     * Elementi göster/gizle
     * @param {Element} el
     * @param {boolean} [gorunur]
     */
    function gorunurluk(el, gorunur) {
        if (!el) return;
        if (gorunur === undefined) {
            el.hidden = !el.hidden;
        } else {
            el.hidden = !gorunur;
        }
    }

    /**
     * Animation frame ile DOM güncellemesi
     * @param {Function} guncelleme
     */
    function domGuncelle(guncelleme) {
        requestAnimationFrame(guncelleme);
    }

    /**
     * Olay dinleyicisi ekle (otomatik temizleme desteği)
     * @param {Element|EventTarget} hedef
     * @param {string}              tur
     * @param {Function}            isleyici
     * @param {Object}              [secenekler]
     * @returns {Function} temizleme fonksiyonu
     */
    function dinle(hedef, tur, isleyici, secenekler = {}) {
        hedef?.addEventListener(tur, isleyici, secenekler);
        return () => hedef?.removeEventListener(tur, isleyici, secenekler);
    }

    /**
     * Delegated event listener — üst elementte olay takibi
     * @param {Element}  ust
     * @param {string}   tur
     * @param {string}   alt_secici
     * @param {Function} isleyici
     * @returns {Function} temizleme
     */
    function delege(ust, tur, alt_secici, isleyici) {
        const ic_isleyici = (event) => {
            const hedef = event.target.closest(alt_secici);
            if (hedef && ust.contains(hedef)) {
                isleyici.call(hedef, event, hedef);
            }
        };
        ust.addEventListener(tur, ic_isleyici);
        return () => ust.removeEventListener(tur, ic_isleyici);
    }

    /**
     * Elementi yaymayı durdur (prevent default + stop propagation)
     * @param {Event} event
     */
    function durdur(event) {
        event.preventDefault();
        event.stopPropagation();
    }

    /**
     * Elementin metnini güvenli şekilde ayarla
     * @param {Element} el
     * @param {string}  metin
     */
    function metin(el, yeniMetin) {
        if (el) el.textContent = yeniMetin;
    }

    /**
     * Elementin attribute'unu al/ayarla
     * @param {Element} el
     * @param {string}  isim
     * @param {string}  [deger]
     * @returns {string|null}
     */
    function attr(el, isim, deger) {
        if (!el) return null;
        if (deger === undefined) return el.getAttribute(isim);
        if (deger === null) el.removeAttribute(isim);
        else el.setAttribute(isim, deger);
        return null;
    }

    /**
     * ARIA attribute yönetimi
     * @param {Element} el
     * @param {Object}  ariaOzellikler
     */
    function aria(el, ariaOzellikler) {
        if (!el) return;
        for (const [anahtar, deger] of Object.entries(ariaOzellikler)) {
            el.setAttribute(`aria-${anahtar}`, String(deger));
        }
    }

    /**
     * Elementi sayfa yapısından kaldır
     * @param {Element|null} el
     */
    function kaldir(el) {
        el?.parentNode?.removeChild(el);
    }

    /**
     * Elementi boşalt
     * @param {Element} el
     */
    function bosalt(el) {
        if (el) el.innerHTML = "";
    }

    /**
     * Odağı yönet — erişilebilirlik
     * @param {Element} el
     */
    function odakla(el) {
        if (!el) return;
        if (el.tabIndex < 0) el.setAttribute("tabindex", "-1");
        el.focus({ preventScroll: true });
    }

    /**
     * Yakalanabilir (focusable) elementleri bul
     * @param {Element} kapsam
     * @returns {Element[]}
     */
    function odaklanabilirler(kapsam) {
        const secici = [
            "a[href]", "button:not([disabled])", "input:not([disabled])",
            "select:not([disabled])", "textarea:not([disabled])",
            "[tabindex]:not([tabindex='-1'])", "[contenteditable='true']",
        ].join(", ");
        return secHepsini(secici, kapsam).filter(el => !el.hidden);
    }

    return Object.freeze({
        sec, secHepsini, olustur, sinif, cssVar, cssVarOku,
        gorunurluk, domGuncelle, dinle, delege, durdur,
        metin, attr, aria, kaldir, bosalt, odakla, odaklanabilirler,
    });
})();

// =============================================================================
// BÖLÜM 7 — RENDER MOTORU
// =============================================================================

class DocsRenderer {
    /**
     * @param {DocsEventEmitter} emitter
     * @param {Object}           [config]
     */
    constructor(emitter, config = {}) {
        this._emitter = emitter;
        this._config = { ...DEFAULT_CONFIG, ...config };
        this._animationFrameId = null;
        this._kuyruk = [];
        this._islemekte = false;
        this._sablon_onbellegi = new DocsCache({ maxBoyut: 50, maxYas: 86400000 });
    }

    /**
     * Render kuyruk ekle
     * @param {Function} gorev
     */
    siraya_ekle(gorev) {
        this._kuyruk.push(gorev);
        if (!this._islemekte) this._islemeBasla();
    }

    /** Kuyruk işlemeyi başlat */
    _islemeBasla() {
        this._islemekte = true;
        this._animationFrameId = requestAnimationFrame(() => this._kuyrukIsle());
    }

    /** Kuyruktaki görevleri işle */
    _kuyrukIsle() {
        const gorevler = this._kuyruk.splice(0);
        for (const gorev of gorevler) {
            try { gorev(); }
            catch (e) { this._emitter.emit(EVENTS.ERROR, { tur: ERROR_TYPES.RENDER, mesaj: e.message }); }
        }
        this._islemekte = false;
    }

    /**
     * İçerik alanını güncelle
     * @param {string}  html
     * @param {Element} kapsam
     */
    icerikGuncelle(html, kapsam) {
        this.siraya_ekle(() => {
            this._emitter.emit(EVENTS.RENDER_START);
            if (kapsam) {
                kapsam.innerHTML = html;
                this._render_sonrasi_islemler(kapsam);
            }
            this._emitter.emit(EVENTS.RENDER_END);
        });
    }

    /**
     * Render sonrası DOM iyileştirmeleri
     * @param {Element} kapsam
     */
    _render_sonrasi_islemler(kapsam) {
        this._kod_bloklari_isle(kapsam);
        this._basliklara_id_ekle(kapsam);
        this._tablo_sarilari_ekle(kapsam);
        this._dis_baglantilar_isle(kapsam);
        this._resim_tembel_yukle(kapsam);
        this._acma_kapama_ekle(kapsam);
    }

    /**
     * Kod bloklarına kopyalama butonu ekle
     * @param {Element} kapsam
     */
    _kod_bloklari_isle(kapsam) {
        DOM.secHepsini("pre code", kapsam).forEach(code => {
            const pre = code.parentElement;
            if (pre.querySelector(".docs-copy-btn")) return;

            const buton = DOM.olustur("button", {
                class: "docs-copy-btn",
                title: "Kodu kopyala",
                "aria-label": "Kodu kopyala",
            });
            buton.textContent = "Kopyala";

            DOM.dinle(buton, "click", async () => {
                const basarili = await Utils.panoyaKopyala(code.textContent);
                buton.textContent = basarili ? "Kopyalandı!" : "Hata!";
                setTimeout(() => { buton.textContent = "Kopyala"; }, 2000);
                this._emitter.emit(EVENTS.CODE_COPY, { icerik: code.textContent });
            });

            pre.style.position = "relative";
            pre.appendChild(buton);
        });
    }

    /**
     * Başlıklara benzersiz ID ekle
     * @param {Element} kapsam
     */
    _basliklara_id_ekle(kapsam) {
        const sayaci = {};
        DOM.secHepsini("h1, h2, h3, h4, h5, h6", kapsam).forEach(baslik => {
            if (baslik.id) return;
            let slug = Utils.slugOlustur(baslik.textContent);
            sayaci[slug] = (sayaci[slug] || 0) + 1;
            if (sayaci[slug] > 1) slug += `-${sayaci[slug]}`;
            baslik.id = slug;
        });
    }

    /**
     * Tabloları sarmalayıcıya al (yatay kaydırma için)
     * @param {Element} kapsam
     */
    _tablo_sarilari_ekle(kapsam) {
        DOM.secHepsini("table", kapsam).forEach(tablo => {
            if (tablo.parentElement?.classList.contains("docs-table-wrapper")) return;
            const sarici = DOM.olustur("div", { class: "docs-table-wrapper" });
            tablo.parentNode.insertBefore(sarici, tablo);
            sarici.appendChild(tablo);
        });
    }

    /**
     * Dış bağlantılara yeni sekme / güvenli rel ekle
     * @param {Element} kapsam
     */
    _dis_baglantilar_isle(kapsam) {
        DOM.secHepsini("a[href]", kapsam).forEach(link => {
            const href = link.getAttribute("href");
            if (!href) return;
            if (href.startsWith("http") || href.startsWith("//")) {
                link.setAttribute("target", "_blank");
                link.setAttribute("rel", "noopener noreferrer");
                link.setAttribute("aria-label",
                    `${link.textContent} (yeni sekmede açılır)`);
                this._emitter.emit(EVENTS.LINK_EXTERNAL, { url: href });
            } else if (href.startsWith("#")) {
                DOM.dinle(link, "click", (e) => {
                    e.preventDefault();
                    const hedef = document.getElementById(href.slice(1));
                    if (hedef) Utils.yumusak_scroll(hedef);
                    this._emitter.emit(EVENTS.LINK_ANCHOR, { hedef: href });
                });
            }
        });
    }

    /**
     * Tembel resim yükleme
     * @param {Element} kapsam
     */
    _resim_tembel_yukle(kapsam) {
        DOM.secHepsini("img[data-src]", kapsam).forEach(resim => {
            Utils.kesisme_gozlemle(resim, (kayitlar) => {
                for (const kayit of kayitlar) {
                    if (kayit.isIntersecting) {
                        resim.src = resim.dataset.src;
                        delete resim.dataset.src;
                    }
                }
            }, { threshold: DEFAULT_CONFIG.lazyLoadThreshold });
        });
    }

    /**
     * Açma/kapama (details/summary) animasyonu
     * @param {Element} kapsam
     */
    _acma_kapama_ekle(kapsam) {
        DOM.secHepsini("details", kapsam).forEach(details => {
            DOM.dinle(details, "toggle", () => {
                DOM.sinif.ayarla(details, CSS_CLASSES.EXPANDED, details.open);
            });
        });
    }

    /**
     * Yükleme göstergesi aç/kapat
     * @param {boolean} aktif
     */
    yuklemeGoster(aktif) {
        const yukleme = DOM.sec(SELECTORS.LOADING);
        if (yukleme) {
            DOM.gorunurluk(yukleme, aktif);
            DOM.aria(yukleme, { busy: aktif });
        }
        DOM.sinif.ayarla(document.body, CSS_CLASSES.LOADING, aktif);
    }

    /**
     * Hata mesajı göster
     * @param {string} mesaj
     * @param {string} [tur] — "error" | "warning" | "info"
     */
    hataGoster(mesaj, tur = "error") {
        const kutu = DOM.sec(SELECTORS.ERROR_BOX);
        if (!kutu) return;
        kutu.textContent = mesaj;
        kutu.className = `docs-error docs-error--${tur}`;
        DOM.aria(kutu, { live: "assertive", role: "alert" });
        DOM.gorunurluk(kutu, true);
        setTimeout(() => DOM.gorunurluk(kutu, false), 5000);
    }

    /**
     * Bildirim (toast) göster
     * @param {string} mesaj
     * @param {string} [tur]
     * @param {number} [sure] — ms
     */
    bildirimGoster(mesaj, tur = "info", sure = 3000) {
        const toast = DOM.olustur("div", {
            class: `docs-toast docs-toast--${tur}`,
            role: "status",
            "aria-live": "polite",
        });
        toast.textContent = mesaj;
        document.body.appendChild(toast);

        requestAnimationFrame(() => DOM.sinif.ekle(toast, "docs-toast--visible"));

        setTimeout(() => {
            DOM.sinif.kaldir(toast, "docs-toast--visible");
            setTimeout(() => DOM.kaldir(toast), 300);
        }, sure);
    }

    /**
     * İlerleme çubuğunu güncelle
     * @param {number} yuzde — 0-100
     */
    ilerlemeGuncelle(yuzde) {
        const bar = DOM.sec(SELECTORS.PROGRESS_BAR);
        if (bar) {
            bar.style.width = `${Math.min(100, Math.max(0, yuzde))}%`;
            DOM.aria(bar, { valuenow: yuzde });
        }
    }

    /** Mevcut animasyon frame'ini iptal et */
    durdur() {
        if (this._animationFrameId) {
            cancelAnimationFrame(this._animationFrameId);
            this._animationFrameId = null;
        }
    }
}

// =============================================================================
// BÖLÜM 8 — ARAMA MOTORU
// =============================================================================

class DocsSearchEngine {
    /**
     * @param {DocsEventEmitter} emitter
     * @param {Object}           [config]
     */
    constructor(emitter, config = {}) {
        this._emitter = emitter;
        this._config = { ...DEFAULT_CONFIG, ...config };
        this._indeks = [];
        this._onbellek = new DocsCache({ maxBoyut: 100, maxYas: 300000 });
        this._sonArama = "";

        // Debounce uygulanmış arama
        this.arama = Utils.debounce(
            this._aramaYap.bind(this),
            this._config.searchDebounce
        );
    }

    /**
     * Arama indeksini yükle
     * @param {Array<{id: string, baslik: string, icerik: string, url: string, etiketler?: string[]}>} belgeler
     */
    indeksOlustur(belgeler) {
        this._indeks = belgeler.map(belge => ({
            ...belge,
            _aramaMetni: [
                belge.baslik ?? "",
                belge.icerik ?? "",
                ...(belge.etiketler ?? []),
            ].join(" ").toLowerCase(),
        }));
    }

    /**
     * Belge ekle
     * @param {Object} belge
     */
    belgeEkle(belge) {
        const varmi = this._indeks.findIndex(b => b.id === belge.id);
        const hazir = {
            ...belge,
            _aramaMetni: [belge.baslik ?? "", belge.icerik ?? ""].join(" ").toLowerCase(),
        };
        if (varmi >= 0) this._indeks[varmi] = hazir;
        else this._indeks.push(hazir);
        this._onbellek.clear();
    }

    /**
     * Belge kaldır
     * @param {string} id
     */
    belgeKaldir(id) {
        this._indeks = this._indeks.filter(b => b.id !== id);
        this._onbellek.clear();
    }

    /**
     * Arama yapan iç fonksiyon
     * @param {string}  sorgu
     * @param {Object}  [secenekler]
     * @returns {Array}
     */
    _aramaYap(sorgu, secenekler = {}) {
        sorgu = sorgu?.trim().toLowerCase();
        if (!sorgu || sorgu.length < 2) {
            this._emitter.emit(EVENTS.SEARCH_CLEAR);
            return [];
        }
        this._sonArama = sorgu;

        // Önbellek kontrolü
        const onbellekAnahtari = `arama:${sorgu}:${JSON.stringify(secenekler)}`;
        if (this._onbellek.has(onbellekAnahtari)) {
            const onbellekSonucu = this._onbellek.get(onbellekAnahtari);
            this._emitter.emit(EVENTS.SEARCH, { sorgu, sonuclar: onbellekSonucu, onbellek: true });
            return onbellekSonucu;
        }

        const kelimeler = sorgu.split(/\s+/).filter(Boolean);
        const limit = secenekler.limit ?? this._config.searchResultsLimit;
        const sonuclar = [];

        for (const belge of this._indeks) {
            const skor = this._skor_hesapla(belge, kelimeler, secenekler);
            if (skor > 0) {
                sonuclar.push({
                    ...belge,
                    _skor: skor,
                    _parlat: kelimeler,
                });
            }
        }

        // Skora göre sırala
        sonuclar.sort((a, b) => b._skor - a._skor);
        const sinirli = sonuclar.slice(0, limit);

        this._onbellek.set(onbellekAnahtari, sinirli);
        this._emitter.emit(EVENTS.SEARCH, { sorgu, sonuclar: sinirli, onbellek: false });

        return sinirli;
    }

    /**
     * Sonuç skoru hesapla
     * @param {Object}   belge
     * @param {string[]} kelimeler
     * @param {Object}   secenekler
     * @returns {number}
     */
    _skor_hesapla(belge, kelimeler, secenekler) {
        let skor = 0;
        const baslik = (belge.baslik ?? "").toLowerCase();
        const icerik = (belge.icerik ?? "").toLowerCase();
        const etiketler = (belge.etiketler ?? []).join(" ").toLowerCase();

        for (const kelime of kelimeler) {
            // Başlık: +10 puan (tam eşleşme)
            if (baslik.includes(kelime)) skor += 10;
            if (baslik.startsWith(kelime)) skor += 5;

            // İçerik: oluşum sayısına göre
            let idx = -1;
            let sayac = 0;
            while ((idx = icerik.indexOf(kelime, idx + 1)) !== -1) sayac++;
            skor += Math.min(sayac, 10);

            // Etiket: +3 puan
            if (etiketler.includes(kelime)) skor += 3;
        }

        // Tüm kelimeler mevcutsa bonus
        if (kelimeler.every(k => belge._aramaMetni.includes(k))) skor += 20;

        // Tam ifade eşleşmesi bonusu
        const tamIfade = kelimeler.join(" ");
        if (baslik.includes(tamIfade)) skor += 15;
        if (icerik.includes(tamIfade)) skor += 8;

        return skor;
    }

    /**
     * Gelişmiş filtreli arama
     * @param {string} sorgu
     * @param {Object} filtreler — {etiket, kategori, tarihAralik}
     * @returns {Array}
     */
    filtreliArama(sorgu, filtreler = {}) {
        let sonuclar = this._aramaYap(sorgu);
        if (filtreler.etiket) {
            sonuclar = sonuclar.filter(b =>
                b.etiketler?.includes(filtreler.etiket)
            );
        }
        if (filtreler.kategori) {
            sonuclar = sonuclar.filter(b => b.kategori === filtreler.kategori);
        }
        if (filtreler.tarihAralik) {
            const { baslangic, bitis } = filtreler.tarihAralik;
            sonuclar = sonuclar.filter(b => {
                const tarih = new Date(b.guncellenmeTarihi);
                return tarih >= baslangic && tarih <= bitis;
            });
        }
        return sonuclar;
    }

    /**
     * Öneriler (autocomplete)
     * @param {string} sorgu
     * @param {number} [limit]
     * @returns {string[]}
     */
    oneri(sorgu, limit = 5) {
        if (!sorgu || sorgu.length < 1) return [];
        const temiz = sorgu.toLowerCase();
        return this._indeks
            .filter(b => b.baslik.toLowerCase().startsWith(temiz))
            .slice(0, limit)
            .map(b => b.baslik);
    }

    /**
     * Popüler aramaları döndür
     * @returns {string[]}
     */
    populerAramalar() {
        return Utils.depolama.oku("docs_populer_aramalar", []);
    }

    /**
     * Arama geçmişini kaydet
     * @param {string} sorgu
     */
    gecmisKaydet(sorgu) {
        if (!sorgu) return;
        let gecmis = Utils.depolama.oku("docs_arama_gecmisi", []);
        gecmis = [sorgu, ...gecmis.filter(g => g !== sorgu)].slice(0, 20);
        Utils.depolama.yaz("docs_arama_gecmisi", gecmis);
    }

    /**
     * Arama geçmişini al
     * @returns {string[]}
     */
    gecmisAl() {
        return Utils.depolama.oku("docs_arama_gecmisi", []);
    }

    /** İndeks istatistikleri */
    istatistik() {
        return {
            belgeSayisi: this._indeks.length,
            onbellekBoyut: this._onbellek.istatistik().boyut,
            sonArama: this._sonArama,
        };
    }
}

// =============================================================================
// BÖLÜM 9 — NAVİGASYON
// =============================================================================

class DocsNavigator {
    /**
     * @param {DocsEventEmitter} emitter
     * @param {Object}           [config]
     */
    constructor(emitter, config = {}) {
        this._emitter = emitter;
        this._config = { ...DEFAULT_CONFIG, ...config };
        this._gecmis = [];
        this._ileri = [];
        this._mevcut = null;
        this._onbellek = new DocsCache();
    }

    /**
     * URL'ye git
     * @param {string}  url
     * @param {boolean} [gecmiseEkle] — tarayıcı geçmişine eklensin mi
     * @returns {Promise<boolean>}
     */
    async git(url, gecmiseEkle = true) {
        if (!url) return false;
        const temizURL = this._url_normalize(url);
        if (temizURL === this._mevcut) return false;

        if (this._mevcut) {
            this._gecmis.push(this._mevcut);
            if (this._gecmis.length > this._config.maxHistorySize) {
                this._gecmis.shift();
            }
        }
        this._ileri = [];
        this._mevcut = temizURL;

        if (gecmiseEkle && history.pushState) {
            history.pushState({ url: temizURL }, "", temizURL);
        }

        this._emitter.emit(EVENTS.NAVIGATE, { url: temizURL });
        return true;
    }

    /**
     * Geri git
     * @returns {string|null}
     */
    geri() {
        if (this._gecmis.length === 0) return null;
        const onceki = this._gecmis.pop();
        if (this._mevcut) this._ileri.push(this._mevcut);
        this._mevcut = onceki;
        this._emitter.emit(EVENTS.NAVIGATE_BACK, { url: onceki });
        return onceki;
    }

    /**
     * İleri git
     * @returns {string|null}
     */
    ileri() {
        if (this._ileri.length === 0) return null;
        const sonraki = this._ileri.pop();
        if (this._mevcut) this._gecmis.push(this._mevcut);
        this._mevcut = sonraki;
        this._emitter.emit(EVENTS.NAVIGATE_FWD, { url: sonraki });
        return sonraki;
    }

    /**
     * Geri gidilebilir mi
     * @returns {boolean}
     */
    geriMumkunMu() { return this._gecmis.length > 0; }

    /**
     * İleri gidilebilir mi
     * @returns {boolean}
     */
    ileriMumkunMu() { return this._ileri.length > 0; }

    /**
     * Mevcut URL
     * @returns {string|null}
     */
    mevcutURL() { return this._mevcut; }

    /**
     * Tüm geçmişi döndür
     * @returns {string[]}
     */
    tamGecmis() { return [...this._gecmis, this._mevcut].filter(Boolean); }

    /**
     * Breadcrumb oluştur
     * @param {string} url
     * @param {Object} yapilandirma — { ayirici, kok }
     * @returns {Array<{baslik: string, url: string}>}
     */
    breadcrumbOlustur(url, yapilandirma = {}) {
        const { ayirici = "/", kok = "/" } = yapilandirma;
        const parcalar = url.replace(/^\/+|\/+$/g, "").split(ayirici);
        const kisimler = [{ baslik: "Ana Sayfa", url: kok }];
        let biriktir = "";
        for (const parca of parcalar) {
            if (!parca) continue;
            biriktir += `/${parca}`;
            kisimler.push({
                baslik: parca.replace(/-/g, " ").replace(/\b\w/g, c => c.toUpperCase()),
                url: biriktir,
            });
        }
        return kisimler;
    }

    /**
     * URL'yi normalleştir
     * @param {string} url
     * @returns {string}
     */
    _url_normalize(url) {
        url = url.trim();
        if (!url.startsWith("/") && !url.startsWith("http")) {
            url = `/${url}`;
        }
        return url.replace(/\/+/g, "/");
    }

    /**
     * Browser geçmişi (popstate) ile senkronizasyon
     */
    popstateBaglantisi() {
        DOM.dinle(window, "popstate", (event) => {
            const url = event.state?.url ?? window.location.pathname;
            this._mevcut = url;
            this._emitter.emit(EVENTS.NAVIGATE, { url, popstate: true });
        });
    }

    /** Geçmişi temizle */
    temizle() {
        this._gecmis = [];
        this._ileri = [];
    }
}

// =============================================================================
// BÖLÜM 10 — VERSİYON YÖNETİMİ
// =============================================================================

class DocsVersionManager {
    /**
     * @param {DocsEventEmitter} emitter
     */
    constructor(emitter) {
        this._emitter = emitter;
        this._surumler = new Map();
        this._mevcut = null;
    }

    /**
     * Sürüm ekle
     * @param {Object} surum — { id, etiket, tarih, url, aktif?, notlar? }
     */
    surumEkle(surum) {
        this._surumler.set(surum.id, {
            id: surum.id,
            etiket: surum.etiket,
            tarih: new Date(surum.tarih),
            url: surum.url,
            aktif: surum.aktif ?? false,
            notlar: surum.notlar ?? "",
        });
        if (surum.aktif) this._mevcut = surum.id;
    }

    /**
     * Aktif sürümü değiştir
     * @param {string} id
     * @returns {boolean}
     */
    surumuDegistir(id) {
        if (!this._surumler.has(id)) return false;
        const onceki = this._mevcut;
        this._mevcut = id;
        this._emitter.emit(EVENTS.VERSION_CHANGE, {
            yeni: id,
            eski: onceki,
            surum: this._surumler.get(id),
        });
        return true;
    }

    /**
     * Mevcut sürüm bilgisi
     * @returns {Object|null}
     */
    mevcutSurum() {
        return this._mevcut ? this._surumler.get(this._mevcut) : null;
    }

    /**
     * Tüm sürümleri listele
     * @returns {Array}
     */
    tumSurumler() {
        return [...this._surumler.values()].sort((a, b) => b.tarih - a.tarih);
    }

    /**
     * En son sürüm
     * @returns {Object|null}
     */
    enSonSurum() {
        const surumler = this.tumSurumler();
        return surumler.length > 0 ? surumler[0] : null;
    }

    /**
     * Sürüm karşılaştırma — semantic versioning
     * @param {string} v1
     * @param {string} v2
     * @returns {-1|0|1}
     */
    static karsilastir(v1, v2) {
        const parcala = (v) => v.split(".").map(Number);
        const [a1, a2, a3] = parcala(v1);
        const [b1, b2, b3] = parcala(v2);
        if (a1 !== b1) return a1 > b1 ? 1 : -1;
        if (a2 !== b2) return a2 > b2 ? 1 : -1;
        if (a3 !== b3) return a3 > b3 ? 1 : -1;
        return 0;
    }

    /**
     * Sürüm notlarını biçimlendir
     * @param {string} id
     * @returns {string}
     */
    notlariBicimle(id) {
        const surum = this._surumler.get(id);
        if (!surum) return "";
        return [
            `## ${surum.etiket}`,
            `Tarih: ${Utils.tarihFormatla(surum.tarih)}`,
            "",
            surum.notlar,
        ].join("\n");
    }

    /**
     * Sürüm seçim HTML'i
     * @returns {string}
     */
    secimHTMLi() {
        return this.tumSurumler().map(s =>
            `<option value="${Utils.htmlKacis(s.id)}"${s.id === this._mevcut ? " selected" : ""}>
                ${Utils.htmlKacis(s.etiket)}
             </option>`
        ).join("");
    }
}

// =============================================================================
// BÖLÜM 11 — TEMA VE GÖRÜNÜM
// =============================================================================

class DocsThemeManager {
    /**
     * @param {DocsEventEmitter} emitter
     * @param {Object}           [config]
     */
    constructor(emitter, config = {}) {
        this._emitter = emitter;
        this._config = { ...DEFAULT_CONFIG, ...config };
        this._mevcut = this._config.theme;
        this._fontBoyutu = this._config.fontSize;
        this._ongoruntuTercihi = this._sistem_temasi();
        this._temalar = new Map([
            ["light", { etiket: "Açık", simge: "☀️" }],
            ["dark", { etiket: "Koyu", simge: "🌙" }],
            ["sepia", { etiket: "Sepya", simge: "📜" }],
            ["contrast", { etiket: "Yüksek Kontrast", simge: "⬛" }],
            ["auto", { etiket: "Sistem", simge: "💻" }],
        ]);
    }

    /**
     * Temayı uygula
     * @param {string} tema
     */
    temaUygula(tema) {
        const etkin = tema === "auto" ? this._ongoruntuTercihi : tema;

        // Eski tema sınıflarını kaldır
        for (const [t] of this._temalar) {
            DOM.sinif.kaldir(document.documentElement, `docs-theme-${t}`);
        }

        DOM.sinif.ekle(document.documentElement, `docs-theme-${etkin}`);
        this._mevcut = tema;

        Utils.depolama.yaz("docs_tema", tema);
        this._emitter.emit(EVENTS.THEME_CHANGE, { tema, etkin });
    }

    /**
     * Tema değiştir
     */
    temaDegistir() {
        const sirali = ["light", "dark", "sepia", "contrast", "auto"];
        const idx = sirali.indexOf(this._mevcut);
        const yeni = sirali[(idx + 1) % sirali.length];
        this.temaUygula(yeni);
    }

    /**
     * Font boyutunu değiştir
     * @param {number} delta — +/- değişim miktarı
     */
    fontBoyutuDegistir(delta) {
        const yeni = Math.max(
            this._config.fontSizeMin,
            Math.min(this._config.fontSizeMax, this._fontBoyutu + delta)
        );
        if (yeni !== this._fontBoyutu) {
            this._fontBoyutu = yeni;
            DOM.cssVar("--docs-font-size", `${yeni}px`);
            document.documentElement.style.fontSize = `${yeni}px`;
            Utils.depolama.yaz("docs_font_boyutu", yeni);
            this._emitter.emit(EVENTS.FONT_CHANGE, { boyut: yeni });
        }
    }

    /**
     * Kaydedilmiş tercihler yükle
     */
    tercihleriYukle() {
        const kaydedilmisTema = Utils.depolama.oku("docs_tema", this._config.theme);
        const kaydedilmisFontBoyutu = Utils.depolama.oku("docs_font_boyutu", this._config.fontSize);
        this.temaUygula(kaydedilmisTema);
        this._fontBoyutu = kaydedilmisFontBoyutu;
        DOM.cssVar("--docs-font-size", `${kaydedilmisFontBoyutu}px`);
    }

    /**
     * Sistem temasını algıla
     * @returns {"light"|"dark"}
     */
    _sistem_temasi() {
        return window.matchMedia?.("(prefers-color-scheme: dark)").matches ? "dark" : "light";
    }

    /** Mevcut temayı döndür */
    mevcutTema() { return this._mevcut; }

    /** Mevcut font boyutunu döndür */
    mevcutFontBoyutu() { return this._fontBoyutu; }

    /**
     * Yazı boyutunu sıfırla
     */
    fontSifirla() {
        this._fontBoyutu = this._config.fontSize;
        DOM.cssVar("--docs-font-size", `${this._fontBoyutu}px`);
        document.documentElement.style.fontSize = `${this._fontBoyutu}px`;
    }

    /**
     * Renk şeması bilgisi
     * @returns {Object}
     */
    renk_semasi() {
        return Object.fromEntries(
            [...this._temalar].map(([id, bilgi]) => [id, {
                ...bilgi,
                aktif: id === this._mevcut,
            }])
        );
    }

    /**
     * Sistem teması değişim dinleyicisi
     */
    sistemTemasiDinle() {
        window.matchMedia?.("(prefers-color-scheme: dark)")
            .addEventListener("change", (e) => {
                this._ongoruntuTercihi = e.matches ? "dark" : "light";
                if (this._mevcut === "auto") this.temaUygula("auto");
            });
    }
}

// =============================================================================
// BÖLÜM 12 — KISAYOL TUŞLARI
// =============================================================================

class DocsHotkeyManager {
    /**
     * @param {DocsEventEmitter} emitter
     * @param {Object}           [config]
     */
    constructor(emitter, config = {}) {
        this._emitter = emitter;
        this._config = { ...DEFAULT_CONFIG, ...config };
        this._kisayollar = new Map();
        this._etkin = this._config.hotkeys;
        this._temizleme = null;
    }

    /**
     * Kisayol tuşu kaydet
     * @param {string}   kombinasyon — örn: "Ctrl+K", "Escape", "F1"
     * @param {Function} isleyici
     * @param {string}   [aciklama]
     * @returns {this}
     */
    kaydet(kombinasyon, isleyici, aciklama = "") {
        const key = kombinasyon.toLowerCase();
        this._kisayollar.set(key, { isleyici, aciklama, kombinasyon });
        return this;
    }

    /**
     * Kisayol tuşu kaldır
     * @param {string} kombinasyon
     */
    kaldir(kombinasyon) {
        this._kisayollar.delete(kombinasyon.toLowerCase());
    }

    /** Dinleyiciyi başlat */
    baslat() {
        if (this._temizleme) return;
        this._temizleme = DOM.dinle(document, "keydown", (event) => {
            if (!this._etkin) return;
            const kombinasyon = this._kombinasyon_olustur(event);
            const kisayol = this._kisayollar.get(kombinasyon);
            if (kisayol) {
                event.preventDefault();
                try {
                    kisayol.isleyici(event);
                    this._emitter.emit(EVENTS.HOTKEY, { kombinasyon });
                } catch (e) {
                    console.error("Docs: Kısayol hatası →", e);
                }
            }
        });
    }

    /** Dinleyiciyi durdur */
    durdur() {
        if (this._temizleme) {
            this._temizleme();
            this._temizleme = null;
        }
    }

    /**
     * Kombinasyon dizesi oluştur
     * @param {KeyboardEvent} event
     * @returns {string}
     */
    _kombinasyon_olustur(event) {
        const parcalar = [];
        if (event.ctrlKey || event.metaKey) parcalar.push("ctrl");
        if (event.altKey) parcalar.push("alt");
        if (event.shiftKey) parcalar.push("shift");
        parcalar.push(event.key.toLowerCase());
        return parcalar.join("+");
    }

    /**
     * Tüm kısayolları listele
     * @returns {Array<{kombinasyon: string, aciklama: string}>}
     */
    listele() {
        return [...this._kisayollar.values()].map(({ kombinasyon, aciklama }) => ({
            kombinasyon, aciklama,
        }));
    }

    /** Kısayolları etkinleştir/devre dışı bırak */
    toggle() {
        this._etkin = !this._etkin;
        return this._etkin;
    }
}

// =============================================================================
// BÖLÜM 13 — AĞ KATMANI (Network)
// =============================================================================

class DocsNetworkClient {
    /**
     * @param {Object} [config]
     */
    constructor(config = {}) {
        this._config = { ...DEFAULT_CONFIG, ...config };
        this._onbellek = new DocsCache({ maxBoyut: 200 });
        this._kuyruk = new Map();
        this._cevrimdisi = !navigator.onLine;

        DOM.dinle(window, "online", () => { this._cevrimdisi = false; });
        DOM.dinle(window, "offline", () => { this._cevrimdisi = true; });
    }

    /**
     * GET isteği
     * @param {string} url
     * @param {Object} [secenekler]
     * @returns {Promise<*>}
     */
    async get(url, secenekler = {}) {
        const onbellekAnahtari = `GET:${url}`;

        // Önbellek kontrolü
        if (secenekler.onbellek !== false && this._onbellek.has(onbellekAnahtari)) {
            return this._onbellek.get(onbellekAnahtari);
        }

        // Tekrarlı isteği birleştir
        if (this._kuyruk.has(onbellekAnahtari)) {
            return this._kuyruk.get(onbellekAnahtari);
        }

        const istek = this._istekYap("GET", url, null, secenekler);
        this._kuyruk.set(onbellekAnahtari, istek);

        try {
            const yanit = await istek;
            this._onbellek.set(onbellekAnahtari, yanit);
            return yanit;
        } finally {
            this._kuyruk.delete(onbellekAnahtari);
        }
    }

    /**
     * POST isteği
     * @param {string} url
     * @param {*}      veri
     * @param {Object} [secenekler]
     * @returns {Promise<*>}
     */
    async post(url, veri, secenekler = {}) {
        return this._istekYap("POST", url, veri, secenekler);
    }

    /**
     * PUT isteği
     * @param {string} url
     * @param {*}      veri
     * @param {Object} [secenekler]
     */
    async put(url, veri, secenekler = {}) {
        return this._istekYap("PUT", url, veri, secenekler);
    }

    /**
     * DELETE isteği
     * @param {string} url
     * @param {Object} [secenekler]
     */
    async delete(url, secenekler = {}) {
        return this._istekYap("DELETE", url, null, secenekler);
    }

    /**
     * İstek yapan iç fonksiyon (yeniden deneme + zaman aşımı)
     * @param {string} metot
     * @param {string} url
     * @param {*}      [veri]
     * @param {Object} [secenekler]
     * @returns {Promise<*>}
     */
    async _istekYap(metot, url, veri, secenekler = {}) {
        const { retryCount, retryDelay, networkTimeout } = this._config;
        const basliklar = {
            "Content-Type": "application/json",
            "X-Docs-Version": DOCS_META.VERSION,
            ...(secenekler.basliklar ?? {}),
        };

        let sonHata;
        for (let deneme = 0; deneme <= retryCount; deneme++) {
            if (deneme > 0) await Utils.bekle(retryDelay * deneme);
            try {
                const kontrolcu = new AbortController();
                const zamanlayici = setTimeout(() => kontrolcu.abort(), networkTimeout);

                const yanit = await fetch(url, {
                    method: metot,
                    headers: basliklar,
                    body: veri ? JSON.stringify(veri) : undefined,
                    signal: kontrolcu.signal,
                });
                clearTimeout(zamanlayici);

                if (!yanit.ok) {
                    throw Object.assign(new Error(`HTTP ${yanit.status}`), {
                        status: yanit.status,
                        tur: ERROR_TYPES.NETWORK,
                    });
                }

                const icerikTipi = yanit.headers.get("Content-Type") ?? "";
                if (icerikTipi.includes("application/json")) {
                    return await yanit.json();
                }
                return await yanit.text();

            } catch (hata) {
                sonHata = hata;
                if (hata.name === "AbortError") {
                    throw Object.assign(new Error("Zaman aşımı"), { tur: ERROR_TYPES.TIMEOUT });
                }
                if (deneme === retryCount) break;
            }
        }
        throw sonHata;
    }

    /**
     * Çevrimdışı mı
     * @returns {boolean}
     */
    cevrimdisiMi() { return this._cevrimdisi; }

    /** Ağ önbelleğini temizle */
    onbellekTemizle() { this._onbellek.clear(); }
}

// =============================================================================
// BÖLÜM 14 — GÜVENLİK (Sanitizer)
// =============================================================================

const DocsSanitizer = (() => {
    /** İzin verilen HTML etiketleri */
    const IZINLI_ETIKETLER = new Set([
        "p", "br", "hr", "h1", "h2", "h3", "h4", "h5", "h6",
        "ul", "ol", "li", "dl", "dt", "dd",
        "table", "thead", "tbody", "tfoot", "tr", "th", "td",
        "blockquote", "pre", "code", "kbd", "samp", "var",
        "strong", "em", "b", "i", "s", "u", "mark", "sub", "sup",
        "a", "img", "figure", "figcaption",
        "div", "span", "section", "article", "nav", "aside",
        "header", "footer", "main",
        "details", "summary",
        "abbr", "cite", "q", "time",
    ]);

    /** İzin verilen attribute'lar */
    const IZINLI_ATRIBUTLAR = new Set([
        "class", "id", "href", "src", "alt", "title", "lang",
        "data-*", "aria-*", "role", "colspan", "rowspan",
        "width", "height", "target", "rel",
        "open", "controls", "autoplay", "loop", "muted",
        "type", "start", "reversed",
    ]);

    /** Tehlikeli protokoller */
    const TEHLIKELI_PROTOKOLLER = /^(javascript:|data:|vbscript:|file:)/i;

    /**
     * HTML içeriğini temizle
     * @param {string} html
     * @returns {string}
     */
    function temizle(html) {
        const sablon = document.createElement("template");
        sablon.innerHTML = html;
        _dugumuTemizle(sablon.content);
        return sablon.innerHTML;
    }

    /**
     * DOM düğümünü özyinelemeli temizle
     * @param {Node} dugum
     */
    function _dugumuTemizle(dugum) {
        const silinecekler = [];

        for (const cocuk of dugum.childNodes) {
            if (cocuk.nodeType === Node.TEXT_NODE) continue;

            if (cocuk.nodeType === Node.ELEMENT_NODE) {
                const etiket = cocuk.tagName.toLowerCase();

                if (!IZINLI_ETIKETLER.has(etiket)) {
                    silinecekler.push(cocuk);
                    continue;
                }

                // Attribute temizleme
                const silinecekAttr = [];
                for (const attr of cocuk.attributes) {
                    const ad = attr.name.toLowerCase();
                    const deger = attr.value;

                    const izinliMi = [...IZINLI_ATRIBUTLAR].some(izinli => {
                        if (izinli.endsWith("*")) {
                            return ad.startsWith(izinli.slice(0, -1));
                        }
                        return ad === izinli;
                    });

                    if (!izinliMi) {
                        silinecekAttr.push(ad);
                    } else if ((ad === "href" || ad === "src") && TEHLIKELI_PROTOKOLLER.test(deger)) {
                        silinecekAttr.push(ad);
                    } else if (ad === "target" && deger !== "_blank") {
                        cocuk.setAttribute("target", "_blank");
                    }
                }

                for (const attr of silinecekAttr) cocuk.removeAttribute(attr);

                // rel ekle — güvenlik
                if (cocuk.tagName === "A" && cocuk.getAttribute("target") === "_blank") {
                    cocuk.setAttribute("rel", "noopener noreferrer");
                }

                _dugumuTemizle(cocuk);
            } else {
                silinecekler.push(cocuk);
            }
        }

        for (const el of silinecekler) {
            if (el.parentNode === dugum) dugum.removeChild(el);
        }
    }

    /**
     * URL güvenlik kontrolü
     * @param {string} url
     * @returns {string|null} — güvensiz ise null
     */
    function guvenliURL(url) {
        if (!url) return null;
        const temiz = url.trim();
        if (TEHLIKELI_PROTOKOLLER.test(temiz)) return null;
        return temiz;
    }

    /**
     * Kullanıcı girdisini temizle (form alanları için)
     * @param {string} giris
     * @returns {string}
     */
    function girdiTemizle(giris) {
        return String(giris)
            .replace(/</g, "&lt;")
            .replace(/>/g, "&gt;")
            .replace(/"/g, "&quot;")
            .replace(/'/g, "&#39;")
            .replace(/`/g, "&#96;")
            .trim();
    }

    /**
     * Script enjeksiyonu tespiti
     * @param {string} metin
     * @returns {boolean}
     */
    function scriptVarMi(metin) {
        return /<script|javascript:|on\w+\s*=|eval\s*\(/i.test(metin);
    }

    return Object.freeze({ temizle, guvenliURL, girdiTemizle, scriptVarMi });
})();

// =============================================================================
// BÖLÜM 15 — EKLENTİ SİSTEMİ (Plugin)
// =============================================================================

class DocsPluginSystem {
    /**
     * @param {DocsEventEmitter} emitter
     */
    constructor(emitter) {
        this._emitter = emitter;
        this._eklentiler = new Map();
        this._kancalar = new Map();
    }

    /**
     * Eklenti yükle
     * @param {Object} eklenti — { isim, surum, yukle, kaldir, kancalar? }
     * @returns {Promise<boolean>}
     */
    async yukle(eklenti) {
        if (!eklenti?.isim) throw new Error("Eklenti ismi zorunludur.");
        if (this._eklentiler.has(eklenti.isim)) {
            console.warn(`Docs: "${eklenti.isim}" eklentisi zaten yüklü.`);
            return false;
        }

        try {
            await eklenti.yukle?.({ emitter: this._emitter, kancalar: this });
            this._eklentiler.set(eklenti.isim, {
                ...eklenti,
                yuklenmeZamani: Date.now(),
            });
            if (eklenti.kancalar) {
                for (const [kanca, fn] of Object.entries(eklenti.kancalar)) {
                    this.kancaEkle(kanca, fn);
                }
            }
            this._emitter.emit(EVENTS.PLUGIN_LOAD, { isim: eklenti.isim });
            return true;
        } catch (e) {
            throw Object.assign(new Error(`Eklenti yükleme hatası: ${e.message}`), {
                tur: ERROR_TYPES.PLUGIN,
            });
        }
    }

    /**
     * Eklenti kaldır
     * @param {string} isim
     * @returns {Promise<boolean>}
     */
    async kaldir(isim) {
        const eklenti = this._eklentiler.get(isim);
        if (!eklenti) return false;
        try {
            await eklenti.kaldir?.({ emitter: this._emitter });
            this._eklentiler.delete(isim);
            this._emitter.emit(EVENTS.PLUGIN_UNLOAD, { isim });
            return true;
        } catch (e) {
            console.error(`Docs: "${isim}" kaldırma hatası →`, e);
            return false;
        }
    }

    /**
     * Kanca ekle
     * @param {string}   isim
     * @param {Function} fn
     */
    kancaEkle(isim, fn) {
        if (!this._kancalar.has(isim)) this._kancalar.set(isim, []);
        this._kancalar.get(isim).push(fn);
    }

    /**
     * Kancayı çalıştır
     * @param {string} isim
     * @param {*}      [veri]
     * @returns {Promise<*>}
     */
    async kancaCalistir(isim, veri) {
        const kancalar = this._kancalar.get(isim) ?? [];
        let sonuc = veri;
        for (const fn of kancalar) {
            try { sonuc = (await fn(sonuc)) ?? sonuc; }
            catch (e) { console.error(`Docs: Kanca hatası [${isim}] →`, e); }
        }
        return sonuc;
    }

    /**
     * Yüklü eklentileri listele
     * @returns {Array}
     */
    listele() {
        return [...this._eklentiler.values()].map(({ isim, surum, yuklenmeZamani }) => ({
            isim, surum, yuklenmeZamani,
        }));
    }

    /**
     * Eklenti yüklü mü
     * @param {string} isim
     * @returns {boolean}
     */
    yukluMu(isim) {
        return this._eklentiler.has(isim);
    }
}

// =============================================================================
// BÖLÜM 16 — ERİŞİLEBİLİRLİK (A11y)
// =============================================================================

const DocsA11y = (() => {
    /**
     * Odak tuzağı — modal/dialog için
     * @param {Element}  kapsam
     * @returns {Function} temizleme fonksiyonu
     */
    function odakTuzagi(kapsam) {
        const odaklanabilirler = DOM.odaklanabilirler(kapsam);
        if (odaklanabilirler.length === 0) return () => { };

        const ilk = odaklanabilirler[0];
        const son = odaklanabilirler[odaklanabilirler.length - 1];

        const temizleme = DOM.dinle(kapsam, "keydown", (event) => {
            if (event.key !== "Tab") return;
            if (event.shiftKey) {
                if (document.activeElement === ilk) {
                    event.preventDefault();
                    son.focus();
                }
            } else {
                if (document.activeElement === son) {
                    event.preventDefault();
                    ilk.focus();
                }
            }
        });

        ilk.focus();
        return temizleme;
    }

    /**
     * Canlı bölge duyurusu — ekran okuyucular için
     * @param {string} mesaj
     * @param {"polite"|"assertive"} [oncelik]
     */
    function duyur(mesaj, oncelik = "polite") {
        let bolge = document.getElementById("docs-a11y-live");
        if (!bolge) {
            bolge = DOM.olustur("div", {
                id: "docs-a11y-live",
                "aria-live": oncelik,
                role: "status",
                style: {
                    position: "absolute", left: "-9999px",
                    width: "1px", height: "1px", overflow: "hidden",
                },
            });
            document.body.appendChild(bolge);
        }
        bolge.textContent = "";
        setTimeout(() => { bolge.textContent = mesaj; }, 100);
    }

    /**
     * Klavye navigasyon modu tespiti
     * @returns {boolean}
     */
    function klavyeModu() {
        return document.body.classList.contains("docs-keyboard-nav");
    }

    /**
     * Klavye/fare modu değiştirici başlat
     */
    function moduBaslat() {
        DOM.dinle(document, "mousedown", () => {
            DOM.sinif.kaldir(document.body, "docs-keyboard-nav");
        });
        DOM.dinle(document, "keydown", (e) => {
            if (e.key === "Tab") DOM.sinif.ekle(document.body, "docs-keyboard-nav");
        });
    }

    /**
     * Renk kontrastını hesapla (WCAG 2.1)
     * @param {string} renk1 — hex
     * @param {string} renk2 — hex
     * @returns {number} — kontrast oranı
     */
    function kontrastOrani(renk1, renk2) {
        const aydinlatma = (hex) => {
            const rgb = [
                parseInt(hex.slice(1, 3), 16),
                parseInt(hex.slice(3, 5), 16),
                parseInt(hex.slice(5, 7), 16),
            ].map(c => {
                const oran = c / 255;
                return oran <= 0.03928 ? oran / 12.92 : Math.pow((oran + 0.055) / 1.055, 2.4);
            });
            return 0.2126 * rgb[0] + 0.7152 * rgb[1] + 0.0722 * rgb[2];
        };
        const l1 = aydinlatma(renk1);
        const l2 = aydinlatma(renk2);
        const parlak = Math.max(l1, l2);
        const karanlik = Math.min(l1, l2);
        return (parlak + 0.05) / (karanlik + 0.05);
    }

    /**
     * WCAG AA standardını karşılıyor mu
     * @param {string} renk1
     * @param {string} renk2
     * @param {boolean} [buyukMetin]
     * @returns {boolean}
     */
    function wcagAA(renk1, renk2, buyukMetin = false) {
        const oran = kontrastOrani(renk1, renk2);
        return buyukMetin ? oran >= 3 : oran >= 4.5;
    }

    return Object.freeze({ odakTuzagi, duyur, klavyeModu, moduBaslat, kontrastOrani, wcagAA });
})();

// =============================================================================
// BÖLÜM 17 — İÇE/DIŞA AKTARMA
// =============================================================================

const DocsExporter = (() => {
    /**
     * İçeriği yazdır
     * @param {Element} [kapsam]
     * @param {string}  [baslik]
     * @param {Object}  [secenekler]
     */
    function yazdir(kapsam, baslik = document.title, secenekler = {}) {
        const geciciPencere = window.open("", "_blank");
        if (!geciciPencere) return;

        const icerik = kapsam?.innerHTML ?? document.body.innerHTML;
        const stiller = [...document.styleSheets]
            .map(ss => {
                try { return [...ss.cssRules].map(r => r.cssText).join("\n"); }
                catch { return ""; }
            })
            .join("\n");

        geciciPencere.document.write(`<!DOCTYPE html>
<html lang="tr">
<head>
<meta charset="UTF-8">
<title>${Utils.htmlKacis(baslik)}</title>
<style>
${stiller}
@media print { .docs-print-hide { display: none !important; } }
</style>
</head>
<body class="docs-print">
<h1>${Utils.htmlKacis(baslik)}</h1>
${icerik}
</body>
</html>`);
        geciciPencere.document.close();
        setTimeout(() => {
            geciciPencere.print();
            geciciPencere.close();
        }, secenekler.gecikme ?? DEFAULT_CONFIG.printDelay);
    }

    /**
     * Markdown olarak dışa aktar
     * @param {string} html
     * @returns {string}
     */
    function htmldenMarkdown(html) {
        let md = html
            .replace(/<h1[^>]*>(.*?)<\/h1>/gi, "# $1\n")
            .replace(/<h2[^>]*>(.*?)<\/h2>/gi, "## $1\n")
            .replace(/<h3[^>]*>(.*?)<\/h3>/gi, "### $1\n")
            .replace(/<h4[^>]*>(.*?)<\/h4>/gi, "#### $1\n")
            .replace(/<h5[^>]*>(.*?)<\/h5>/gi, "##### $1\n")
            .replace(/<h6[^>]*>(.*?)<\/h6>/gi, "###### $1\n")
            .replace(/<strong[^>]*>(.*?)<\/strong>/gi, "**$1**")
            .replace(/<em[^>]*>(.*?)<\/em>/gi, "_$1_")
            .replace(/<code[^>]*>(.*?)<\/code>/gi, "`$1`")
            .replace(/<a[^>]*href="([^"]*)"[^>]*>(.*?)<\/a>/gi, "[$2]($1)")
            .replace(/<img[^>]*src="([^"]*)"[^>]*alt="([^"]*)"[^>]*>/gi, "![$2]($1)")
            .replace(/<br\s*\/?>/gi, "\n")
            .replace(/<p[^>]*>(.*?)<\/p>/gi, "$1\n\n")
            .replace(/<li[^>]*>(.*?)<\/li>/gi, "- $1\n")
            .replace(/<blockquote[^>]*>(.*?)<\/blockquote>/gi, "> $1")
            .replace(/<[^>]+>/g, "")
            .replace(/\n{3,}/g, "\n\n")
            .trim();
        return md;
    }

    /**
     * JSON olarak dışa aktar
     * @param {Object} veri
     * @param {string} [dosyaAdi]
     */
    function jsonIndir(veri, dosyaAdi = "docs-export.json") {
        const blob = new Blob([JSON.stringify(veri, null, 2)], { type: "application/json" });
        _dosyaIndir(blob, dosyaAdi);
    }

    /**
     * Metin dosyası indir
     * @param {string} icerik
     * @param {string} [dosyaAdi]
     * @param {string} [tip]
     */
    function metinIndir(icerik, dosyaAdi = "docs-export.txt", tip = "text/plain") {
        const blob = new Blob([icerik], { type: `${tip};charset=utf-8` });
        _dosyaIndir(blob, dosyaAdi);
    }

    /**
     * Blob'u dosya olarak indir (yardımcı)
     * @param {Blob}   blob
     * @param {string} dosyaAdi
     */
    function _dosyaIndir(blob, dosyaAdi) {
        const url = URL.createObjectURL(blob);
        const link = DOM.olustur("a", { href: url, download: dosyaAdi });
        document.body.appendChild(link);
        link.click();
        document.body.removeChild(link);
        setTimeout(() => URL.revokeObjectURL(url), 1000);
    }

    /**
     * HTML snippet kopyala
     * @param {string} html
     * @returns {Promise<boolean>}
     */
    async function htmlKopyala(html) {
        return Utils.panoyaKopyala(html);
    }

    /**
     * PDF görünümü oluştur (tarayıcı baskı API kullanarak)
     * @param {string} icerik
     * @param {string} [baslik]
     */
    function pdfExport(icerik, baslik = "") {
        yazdir(null, baslik, {});
    }

    return Object.freeze({ yazdir, htmldenMarkdown, jsonIndir, metinIndir, htmlKopyala, pdfExport });
})();

// =============================================================================
// BÖLÜM 18 — TELEMETRİ VE LOGLAMA
// =============================================================================

class DocsLogger {
    /**
     * @param {Object} [config]
     */
    constructor(config = {}) {
        this._seviye = config.seviye ?? LOG_LEVELS.INFO;
        this._etkin = config.etkin ?? DEFAULT_CONFIG.debugMode;
        this._gecmis = [];
        this._maxGecmis = 1000;
        this._onIsleyici = config.onIsleyici ?? null;
    }

    /**
     * @param {number} seviye
     * @param {string} mesaj
     * @param {*}      [veri]
     */
    _logla(seviye, mesaj, veri) {
        if (seviye < this._seviye) return;

        const giris = {
            seviye,
            mesaj,
            veri,
            zaman: new Date().toISOString(),
            sayac: this._gecmis.length + 1,
        };

        this._gecmis.push(giris);
        if (this._gecmis.length > this._maxGecmis) this._gecmis.shift();

        if (this._onIsleyici) {
            try { this._onIsleyici(giris); }
            catch { /* Loglama hatası sessizce yutulur */ }
        }

        if (!this._etkin && seviye < LOG_LEVELS.WARN) return;

        const prefix = `[Docs ${new Date().toLocaleTimeString("tr-TR")}]`;
        switch (seviye) {
            case LOG_LEVELS.DEBUG: console.debug(prefix, mesaj, veri ?? ""); break;
            case LOG_LEVELS.INFO: console.info(prefix, mesaj, veri ?? ""); break;
            case LOG_LEVELS.WARN: console.warn(prefix, mesaj, veri ?? ""); break;
            case LOG_LEVELS.ERROR: console.error(prefix, mesaj, veri ?? ""); break;
        }
    }

    debug(mesaj, veri) { this._logla(LOG_LEVELS.DEBUG, mesaj, veri); }
    info(mesaj, veri) { this._logla(LOG_LEVELS.INFO, mesaj, veri); }
    warn(mesaj, veri) { this._logla(LOG_LEVELS.WARN, mesaj, veri); }
    error(mesaj, veri) { this._logla(LOG_LEVELS.ERROR, mesaj, veri); }

    /**
     * Performans ölçümü başlat
     * @param {string} etiket
     */
    zamanBaslat(etiket) {
        if (typeof performance !== "undefined") {
            performance.mark(`docs-start-${etiket}`);
        }
    }

    /**
     * Performans ölçümünü bitir ve logla
     * @param {string} etiket
     * @returns {number} ms
     */
    zamanBitir(etiket) {
        if (typeof performance === "undefined") return 0;
        try {
            performance.mark(`docs-end-${etiket}`);
            performance.measure(
                `docs-${etiket}`,
                `docs-start-${etiket}`,
                `docs-end-${etiket}`
            );
            const olcum = performance.getEntriesByName(`docs-${etiket}`)[0];
            const sure = olcum?.duration ?? 0;
            this.debug(`⏱ ${etiket}: ${sure.toFixed(2)}ms`);
            return sure;
        } catch { return 0; }
    }

    /** Geçmişi döndür */
    gecmis(seviyeFiltre) {
        if (seviyeFiltre === undefined) return [...this._gecmis];
        return this._gecmis.filter(g => g.seviye === seviyeFiltre);
    }

    /** Geçmişi temizle */
    temizle() { this._gecmis = []; }

    /** Debug modunu aç/kapat */
    debugToggle() {
        this._etkin = !this._etkin;
        return this._etkin;
    }
}

// =============================================================================
// BÖLÜM 19 — İÇİNDEKİLER TABLOSU (TOC)
// =============================================================================

class DocsTOCManager {
    /**
     * @param {DocsEventEmitter} emitter
     * @param {Object}           [config]
     */
    constructor(emitter, config = {}) {
        this._emitter = emitter;
        this._config = { ...DEFAULT_CONFIG, ...config };
        this._gozlemci = null;
        this._mevcutBaslik = null;
    }

    /**
     * TOC oluştur
     * @param {Element} icerikKapsami — içerik alanı
     * @param {Element} tocKapsami    — TOC yerleştirme alanı
     */
    olustur(icerikKapsami, tocKapsami) {
        const basliklar = DOM.secHepsini("h2, h3, h4", icerikKapsami);
        if (basliklar.length < this._config.tocMinHeadings) return;

        const liste = DOM.olustur("nav", {
            class: "docs-toc-nav",
            "aria-label": "İçindekiler",
        });
        const ul = DOM.olustur("ul", { class: "docs-toc-list" });

        let mevcutSeviye = 2;
        let mevcutListe = ul;
        const yiginlar = [ul];

        for (const baslik of basliklar) {
            const seviye = parseInt(baslik.tagName[1]);
            const id = baslik.id || Utils.slugOlustur(baslik.textContent);
            baslik.id = id;

            while (seviye > mevcutSeviye) {
                const altUl = DOM.olustur("ul", { class: "docs-toc-sub" });
                const sonLi = mevcutListe.lastElementChild;
                if (sonLi) sonLi.appendChild(altUl);
                yiginlar.push(altUl);
                mevcutListe = altUl;
                mevcutSeviye++;
            }
            while (seviye < mevcutSeviye && yiginlar.length > 1) {
                yiginlar.pop();
                mevcutListe = yiginlar[yiginlar.length - 1];
                mevcutSeviye--;
            }

            const li = DOM.olustur("li", { class: `docs-toc-item docs-toc-h${seviye}` });
            const link = DOM.olustur("a", { class: "docs-toc-link", href: `#${id}` });
            link.textContent = baslik.textContent;

            DOM.dinle(link, "click", (e) => {
                e.preventDefault();
                Utils.yumusak_scroll(baslik);
                this._emitter.emit(EVENTS.TOC_CLICK, { id, baslik: baslik.textContent });
            });

            li.appendChild(link);
            mevcutListe.appendChild(li);
        }

        liste.appendChild(ul);
        DOM.bosalt(tocKapsami);
        tocKapsami.appendChild(liste);

        if (this._config.tocAutoHighlight) {
            this._aktifVurgula(basliklar, tocKapsami);
        }
    }

    /**
     * Aktif başlığı vurgula (IntersectionObserver ile)
     * @param {Element[]} basliklar
     * @param {Element}   tocKapsami
     */
    _aktifVurgula(basliklar, tocKapsami) {
        if (this._gozlemci) this._gozlemci.disconnect();

        this._gozlemci = new IntersectionObserver(
            (kayitlar) => {
                for (const kayit of kayitlar) {
                    if (kayit.isIntersecting) {
                        const id = kayit.target.id;
                        if (id === this._mevcutBaslik) continue;
                        this._mevcutBaslik = id;

                        // Tüm aktif sınıfları kaldır
                        DOM.secHepsini(".docs-toc-link.docs-active", tocKapsami)
                            .forEach(l => DOM.sinif.kaldir(l, "docs-active"));

                        // Yeni aktif linki vurgula
                        const aktifLink = DOM.sec(`a[href="#${id}"]`, tocKapsami);
                        if (aktifLink) {
                            DOM.sinif.ekle(aktifLink, "docs-active");
                            this._emitter.emit(EVENTS.HEADING_VISIBLE, { id });
                        }
                    }
                }
            },
            { rootMargin: "-80px 0px -66% 0px", threshold: 0 }
        );

        for (const baslik of basliklar) this._gozlemci.observe(baslik);
    }

    /** Gözlemciyi temizle */
    temizle() {
        this._gozlemci?.disconnect();
        this._gozlemci = null;
    }
}

// =============================================================================
// BÖLÜM 20 — ANA DOCS API
// =============================================================================

class DocsAPI {
    /**
     * @param {Object} [kullanici_config]
     */
    constructor(kullanici_config = {}) {
        this._config = Utils.derinBirlestir(DEFAULT_CONFIG, kullanici_config);
        this._baslandi = false;
        this._acik = false;
        this._mevcut_url = null;

        // Alt sistemleri başlat
        this.emitter = new DocsEventEmitter();
        this.logger = new DocsLogger({ etkin: this._config.debugMode });
        this.cache = new DocsCache({ maxBoyut: this._config.cacheMaxSize, maxYas: this._config.cacheMaxAge });
        this.state = new DocsState({
            acik: false,
            url: null,
            tema: this._config.theme,
            fontBoyutu: this._config.fontSize,
            yukleniyor: false,
            hata: null,
            arama: "",
            yer_imleri: [],
            gecmis: [],
        });
        this.renderer = new DocsRenderer(this.emitter, this._config);
        this.search = new DocsSearchEngine(this.emitter, this._config);
        this.navigator = new DocsNavigator(this.emitter, this._config);
        this.theme = new DocsThemeManager(this.emitter, this._config);
        this.hotkeys = new DocsHotkeyManager(this.emitter, this._config);
        this.network = new DocsNetworkClient(this._config);
        this.plugins = new DocsPluginSystem(this.emitter);
        this.toc = new DocsTOCManager(this.emitter, this._config);
        this.versions = new DocsVersionManager(this.emitter);
        this.bookmarks = new DocsBookmarkManager(this.emitter);
        this.animationFrameId = null;
    }

    /**
     * Docs sistemini başlat
     * @param {Function|null} [geri_donus] — başlatma tamamlandığında çağrılır
     * @returns {this}
     */
    initialize(geri_donus = null) {
        if (this._baslandi) {
            this.logger.warn("Docs zaten başlatıldı.");
            return this;
        }

        this.logger.zamanBaslat("init");
        this.emitter.emit(EVENTS.INIT);

        try {
            this._olaylari_bagla();
            this._kisayolları_kaydet();
            this.theme.tercihleriYukle();
            this.theme.sistemTemasiDinle();
            this.navigator.popstateBaglantisi();
            DocsA11y.moduBaslat();

            if (this._config.persistState) {
                this._durumu_yukle();
            }

            this._baslandi = true;
            this.emitter.emit(EVENTS.READY);

            const sure = this.logger.zamanBitir("init");
            this.logger.info(`Docs başlatıldı (${sure.toFixed(0)}ms)`);

            if (typeof geri_donus === "function") geri_donus(null, this);

        } catch (hata) {
            this.logger.error("Başlatma hatası", hata);
            if (typeof geri_donus === "function") geri_donus(hata, this);
        }

        return this;
    }

    /**
     * Döküman aç
     * @param {string} [url]
     * @returns {Promise<void>}
     */
    async openDocumentation(url) {
        url = url ?? this._config.baseUrl;
        const temizURL = DocsSanitizer.guvenliURL(url);
        if (!temizURL) {
            this.logger.warn("Güvenlik: Geçersiz URL reddedildi →", url);
            return;
        }

        this._acik = true;
        this.state.set({ acik: true, url: temizURL, yukleniyor: true });
        this.renderer.yuklemeGoster(true);
        DOM.sinif.ekle(document.documentElement, CSS_CLASSES.OPEN);
        this.emitter.emit(EVENTS.OPEN, { url: temizURL });
        DocsA11y.duyur("Döküman açılıyor…");

        try {
            const icerik = await this._icerik_yukle(temizURL);
            await this.navigator.git(temizURL);
            this._icerik_isle(icerik, temizURL);
            this.state.set({ yukleniyor: false, hata: null });
        } catch (hata) {
            this.logger.error("İçerik yükleme hatası", hata);
            this.state.set({ yukleniyor: false, hata: hata.message });
            this.renderer.hataGoster(`Döküman yüklenemedi: ${hata.message}`);
            this.emitter.emit(EVENTS.LOAD_ERROR, { hata });
        } finally {
            this.renderer.yuklemeGoster(false);
        }
    }

    /**
     * Dökümanı kapat
     */
    closeDocumentation() {
        if (!this._acik) return;
        this._acik = false;
        this.state.set({ acik: false });
        DOM.sinif.kaldir(document.documentElement, CSS_CLASSES.OPEN);
        this.emitter.emit(EVENTS.CLOSE);
        DocsA11y.duyur("Döküman kapatıldı.");
        if (this._config.persistState) this._durumu_kaydet();
    }

    /**
     * İçerik yükle
     * @param {string} url
     * @returns {Promise<string>}
     */
    async _icerik_yukle(url) {
        return this.cache.cacheOrFetch(
            `icerik:${url}`,
            async () => {
                this.emitter.emit(EVENTS.LOAD_START, { url });
                const icerik = await this.network.get(url);
                this.emitter.emit(EVENTS.LOAD_END, { url });
                return icerik;
            }
        );
    }

    /**
     * İçeriği işle ve render et
     * @param {string} icerik
     * @param {string} url
     */
    async _icerik_isle(icerik, url) {
        const temizIcerik = this._config.sanitizeContent
            ? DocsSanitizer.temizle(typeof icerik === "string" ? icerik : icerik.html ?? "")
            : (typeof icerik === "string" ? icerik : icerik.html ?? "");

        const icerikEl = DOM.sec(SELECTORS.CONTENT_AREA);
        if (icerikEl) {
            this.renderer.icerikGuncelle(temizIcerik, icerikEl);
        }

        // Plugin kancası
        await this.plugins.kancaCalistir("icerik_render_sonrasi", { icerik: temizIcerik, url });

        // TOC oluştur
        const tocEl = DOM.sec(SELECTORS.TOC);
        if (tocEl && icerikEl) {
            requestAnimationFrame(() => this.toc.olustur(icerikEl, tocEl));
        }

        // Breadcrumb güncelle
        this._breadcrumb_guncelle(url);

        // Okuma ilerlemesi
        if (this._config.readingProgress) {
            this._okuma_ilerlemesi_baslat();
        }

        this.emitter.emit(EVENTS.RENDER, { url });
        DocsA11y.duyur("Döküman yüklendi.");
    }

    /**
     * Render döngüsü (requestAnimationFrame tabanlı)
     */
    render() {
        if (!this._baslandi) return;
        this.animationFrameId = requestAnimationFrame(() => this.render());
    }

    /**
     * Breadcrumb'ı güncelle
     * @param {string} url
     */
    _breadcrumb_guncelle(url) {
        const breadcrumbEl = DOM.sec(SELECTORS.BREADCRUMB);
        if (!breadcrumbEl) return;

        const kisimlar = this.navigator.breadcrumbOlustur(url, {
            kok: this._config.baseUrl,
        });

        const html = kisimlar.map((k, i) =>
            i === kisimlar.length - 1
                ? `<span aria-current="page">${Utils.htmlKacis(k.baslik)}</span>`
                : `<a href="${Utils.htmlKacis(k.url)}" class="docs-link">${Utils.htmlKacis(k.baslik)}</a>`
        ).join(' <span aria-hidden="true">›</span> ');

        breadcrumbEl.innerHTML = `<nav aria-label="Konum">${html}</nav>`;
    }

    /**
     * Okuma ilerlemesi çubuğu
     */
    _okuma_ilerlemesi_baslat() {
        const guncelleIlerleme = Utils.throttle(() => {
            const el = document.documentElement;
            const giz = el.scrollHeight - el.clientHeight;
            const oran = giz > 0 ? (window.pageYOffset / giz) * 100 : 0;
            this.renderer.ilerlemeGuncelle(oran);
        }, 16);

        DOM.dinle(window, "scroll", guncelleIlerleme, { passive: true });
    }

    /**
     * Olayları bağla
     */
    _olaylari_bagla() {
        // Link tıklamaları — delegasyon
        DOM.delege(document.documentElement, "click", SELECTORS.LINK, (event, link) => {
            DOM.durdur(event);
            const url = link.getAttribute("href") ?? link.dataset.url;
            if (url) this.openDocumentation(url);
        });

        // Kapatma butonu
        DOM.delege(document.documentElement, "click", SELECTORS.CLOSE_BTN, (event) => {
            DOM.durdur(event);
            this.closeDocumentation();
        });

        // Arama
        const aramaInput = DOM.sec(SELECTORS.SEARCH_INPUT);
        if (aramaInput) {
            DOM.dinle(aramaInput, "input", (e) => {
                const sorgu = e.target.value;
                this.state.set({ arama: sorgu });
                this.search.arama(sorgu);
            });
            DOM.dinle(aramaInput, "keydown", (e) => {
                if (e.key === "Escape") {
                    aramaInput.value = "";
                    this.emitter.emit(EVENTS.SEARCH_CLEAR);
                }
            });
        }

        // Tema değiştirme
        DOM.delege(document.documentElement, "click", SELECTORS.THEME_TOGGLE, () => {
            this.theme.temaDegistir();
        });

        // Font boyutu
        DOM.delege(document.documentElement, "click", SELECTORS.FONT_INCREASE, () => {
            this.theme.fontBoyutuDegistir(+1);
        });
        DOM.delege(document.documentElement, "click", SELECTORS.FONT_DECREASE, () => {
            this.theme.fontBoyutuDegistir(-1);
        });

        // Yazdır
        DOM.delege(document.documentElement, "click", SELECTORS.PRINT_BTN, () => {
            const icerikEl = DOM.sec(SELECTORS.CONTENT_AREA);
            DocsExporter.yazdir(icerikEl);
            this.emitter.emit(EVENTS.PRINT);
        });

        // Yer imi
        DOM.delege(document.documentElement, "click", SELECTORS.BOOKMARK_BTN, () => {
            const url = this.state.get("url");
            if (url) this.bookmarks.togglye(url);
        });

        // Paylaş
        DOM.delege(document.documentElement, "click", SELECTORS.SHARE_BTN, async () => {
            const url = window.location.href;
            if (navigator.share) {
                await navigator.share({ title: document.title, url });
            } else {
                await Utils.panoyaKopyala(url);
                this.renderer.bildirimGoster("URL kopyalandı!", "success");
            }
            this.emitter.emit(EVENTS.SHARE, { url });
        });

        // Versiyon değişimi
        DOM.delege(document.documentElement, "change", SELECTORS.VERSION_SELECT, (event, select) => {
            const id = select.value;
            if (id) this.versions.surumuDegistir(id);
        });

        // Geri/İleri navigasyon
        this.emitter.on(EVENTS.NAVIGATE_BACK, ({ url }) => { if (url) this.openDocumentation(url); });
        this.emitter.on(EVENTS.NAVIGATE_FWD, ({ url }) => { if (url) this.openDocumentation(url); });

        // Çevrimiçi/Çevrimdışı
        DOM.dinle(window, "online", () => { this.emitter.emit(EVENTS.ONLINE); this.renderer.bildirimGoster("Bağlantı kuruldu.", "success"); });
        DOM.dinle(window, "offline", () => { this.emitter.emit(EVENTS.OFFLINE); this.renderer.bildirimGoster("Çevrimdışı moda girildi.", "warning"); });

        // Scroll to top
        DOM.delege(document.documentElement, "click", SELECTORS.SCROLL_TOP, () => {
            Utils.yumusak_scroll(0);
        });

        // Scroll görünürlük
        const scrollButon = DOM.sec(SELECTORS.SCROLL_TOP);
        if (scrollButon) {
            DOM.dinle(window, "scroll", Utils.throttle(() => {
                DOM.gorunurluk(scrollButon, window.pageYOffset > this._config.scrollThreshold);
            }, 100), { passive: true });
        }

        // Hata olayı
        this.emitter.on(EVENTS.ERROR, ({ mesaj, tur }) => {
            this.logger.error(`[${tur ?? "?"}] ${mesaj}`);
        });
    }

    /**
     * Kısayol tuşlarını kaydet
     */
    _kisayolları_kaydet() {
        this.hotkeys
            .kaydet("Escape", () => this.closeDocumentation(), "Dökümanı kapat")
            .kaydet("F1", () => this.openDocumentation(), "Dökümanı aç")
            .kaydet("ctrl+k", () => DOM.sec(SELECTORS.SEARCH_INPUT)?.focus(), "Aramaya odaklan")
            .kaydet("ctrl+f", () => DOM.sec(SELECTORS.SEARCH_INPUT)?.focus(), "Ara")
            .kaydet("ctrl+b", () => {
                "Geri git";
                const url = this.navigator.geri();
                if (url) this.openDocumentation(url);
            }, "Geri git")
            .kaydet("ctrl+shift+b", () => {
                const url = this.navigator.ileri();
                if (url) this.openDocumentation(url);
            }, "İleri git")
            .kaydet("ctrl+shift+d", () => {
                const urlOku = this.state.get("url");
                if (urlOku) this.bookmarks.togglye(urlOku);
            }, "Yer imi ekle/kaldır")
            .kaydet("ctrl+p", () => {
                const icerikEl = DOM.sec(SELECTORS.CONTENT_AREA);
                DocsExporter.yazdir(icerikEl);
            }, "Yazdır")
            .kaydet("ctrl+shift+t", () => this.theme.temaDegistir(), "Tema değiştir")
            .kaydet("ctrl+shift+=", () => this.theme.fontBoyutuDegistir(+1), "Yazı boyutunu artır")
            .kaydet("ctrl+shift+-", () => this.theme.fontBoyutuDegistir(-1), "Yazı boyutunu azalt")
            .kaydet("ctrl+shift+0", () => this.theme.fontSifirla(), "Yazı boyutunu sıfırla")
            .kaydet("?", () => this._kisayol_rehberi_goster(), "Yardım");

        this.hotkeys.baslat();
    }

    /**
     * Kısayol rehberini göster
     */
    _kisayol_rehberi_goster() {
        const kisayollar = this.hotkeys.listele();
        const html = `
            <h2>Klavye Kısayolları</h2>
            <table>
                <thead><tr><th>Kısayol</th><th>Açıklama</th></tr></thead>
                <tbody>
                    ${kisayollar.map(k =>
            `<tr>
                            <td><kbd>${Utils.htmlKacis(k.kombinasyon)}</kbd></td>
                            <td>${Utils.htmlKacis(k.aciklama)}</td>
                        </tr>`
        ).join("")}
                </tbody>
            </table>`;
        this.renderer.bildirimGoster("Kısayol rehberi konsolda görüntülendi.", "info");
        console.info("[Docs Klavye Kısayolları]\n" +
            kisayollar.map(k => `  ${k.kombinasyon.padEnd(20)} → ${k.aciklama}`).join("\n")
        );
    }

    /**
     * Durumu LocalStorage'a kaydet
     */
    _durumu_kaydet() {
        const kaydedilecek = {
            tema: this.state.get("tema"),
            fontBoyutu: this.state.get("fontBoyutu"),
            yer_imleri: this.bookmarks.tumunu_al(),
            _surum: this._config.storageVersion,
        };
        Utils.depolama.yaz(this._config.storageKey, kaydedilecek);
        this.emitter.emit(EVENTS.STATE_SAVE, kaydedilecek);
    }

    /**
     * Durumu LocalStorage'dan yükle
     */
    _durumu_yukle() {
        const kaydedilmis = Utils.depolama.oku(this._config.storageKey);
        if (!kaydedilmis || kaydedilmis._surum !== this._config.storageVersion) return;

        this.state.set({
            tema: kaydedilmis.tema ?? this._config.theme,
            fontBoyutu: kaydedilmis.fontBoyutu ?? this._config.fontSize,
        });
        if (kaydedilmis.yer_imleri?.length) {
            this.bookmarks.toplu_yukle(kaydedilmis.yer_imleri);
        }
        this.emitter.emit(EVENTS.STATE_RESTORE, kaydedilmis);
    }

    /**
     * Sistemi durdur ve kaynakları serbest bırak
     */
    destroy() {
        this.hotkeys.durdur();
        this.renderer.durdur();
        this.toc.temizle();
        this.emitter.temizle();
        this.cache.clear();
        if (this._config.persistState) this._durumu_kaydet();
        this._baslandi = false;
        this.emitter.emit(EVENTS.DESTROY);
        this.logger.info("Docs durduruldu.");
    }

    // ── Kısayol API ──────────────────────────────────────────────────────────

    /** Başlatıldı mı */
    get baslandi() { return this._baslandi; }

    /** Açık mı */
    get acikMi() { return this._acik; }

    /** Mevcut URL */
    get mevcutURL() { return this.state.get("url"); }

    /** Versiyon meta bilgisi */
    get meta() { return { ...DOCS_META }; }

    /** Yapılandırma kopyası */
    get yapilandirma() { return { ...this._config }; }
}

// =============================================================================
// BÖLÜM 21 — YER İMİ YÖNETİCİSİ (BookmarkManager)
// =============================================================================

class DocsBookmarkManager {
    /**
     * @param {DocsEventEmitter} emitter
     */
    constructor(emitter) {
        this._emitter = emitter;
        this._yer_imleri = new Map();
    }

    /**
     * Yer imi ekle
     * @param {string}  url
     * @param {string}  [baslik]
     * @param {string}  [aciklama]
     * @returns {Object} — eklenen yer imi
     */
    ekle(url, baslik = "", aciklama = "") {
        if (this._yer_imleri.size >= DEFAULT_CONFIG.maxBookmarks) {
            throw new Error(`Maksimum yer imi sayısına ulaşıldı: ${DEFAULT_CONFIG.maxBookmarks}`);
        }
        const yer_imi = {
            id: Utils.benzersizID("bm"),
            url,
            baslik: baslik || url,
            aciklama,
            eklenmeZamani: Date.now(),
        };
        this._yer_imleri.set(url, yer_imi);
        this._emitter.emit(EVENTS.BOOKMARK_ADD, yer_imi);
        return yer_imi;
    }

    /**
     * Yer imi kaldır
     * @param {string} url
     * @returns {boolean}
     */
    kaldir(url) {
        const silindi = this._yer_imleri.delete(url);
        if (silindi) this._emitter.emit(EVENTS.BOOKMARK_REMOVE, { url });
        return silindi;
    }

    /**
     * Yer imi var mı
     * @param {string} url
     * @returns {boolean}
     */
    varMi(url) { return this._yer_imleri.has(url); }

    /**
     * Ekle veya kaldır (toggle)
     * @param {string} url
     * @param {string} [baslik]
     */
    togglye(url, baslik) {
        if (this.varMi(url)) this.kaldir(url);
        else this.ekle(url, baslik);
    }

    /**
     * Tüm yer imlerini al
     * @returns {Object[]}
     */
    tumunu_al() {
        return [...this._yer_imleri.values()]
            .sort((a, b) => b.eklenmeZamani - a.eklenmeZamani);
    }

    /**
     * Toplu yükle
     * @param {Object[]} yer_imleri
     */
    toplu_yukle(yer_imleri) {
        for (const yi of yer_imleri) {
            if (yi?.url) this._yer_imleri.set(yi.url, yi);
        }
    }

    /**
     * Yer imi sayısı
     * @returns {number}
     */
    sayac() { return this._yer_imleri.size; }

    /**
     * Tüm yer imlerini temizle
     */
    tumunuTemizle() {
        this._yer_imleri.clear();
    }

    /**
     * Yer imlerini ara
     * @param {string} sorgu
     * @returns {Object[]}
     */
    ara(sorgu) {
        const temiz = sorgu.toLowerCase();
        return this.tumunu_al().filter(yi =>
            yi.baslik.toLowerCase().includes(temiz) ||
            yi.url.toLowerCase().includes(temiz)
        );
    }
}

// =============================================================================
// BÖLÜM 22 — GERİ BİLDİRİM YÖNETİCİSİ (FeedbackManager)
// =============================================================================

class DocsFeedbackManager {
    /**
     * @param {DocsNetworkClient} network
     * @param {DocsEventEmitter}  emitter
     * @param {Object}            [config]
     */
    constructor(network, emitter, config = {}) {
        this._network = network;
        this._emitter = emitter;
        this._config = { ...DEFAULT_CONFIG, ...config };
    }

    /**
     * Geri bildirim gönder
     * @param {Object} veri — { url, tur, mesaj, rating?, iletisim? }
     * @returns {Promise<boolean>}
     */
    async gonder(veri) {
        const sema = {
            url: { zorunlu: true, tip: "string" },
            tur: { zorunlu: true, tip: "string" },
            mesaj: { zorunlu: true, tip: "string", minLength: 5, maxLength: 1000 },
        };
        const { gecerli, hatalar } = Utils.semaDogrula(veri, sema);
        if (!gecerli) {
            throw new Error(`Geri bildirim doğrulama hatası: ${hatalar.join(", ")}`);
        }

        const temizVeri = {
            url: DocsSanitizer.girdiTemizle(veri.url),
            tur: DocsSanitizer.girdiTemizle(veri.tur),
            mesaj: DocsSanitizer.girdiTemizle(veri.mesaj),
            rating: veri.rating ?? null,
            tarih: new Date().toISOString(),
            uaAgent: navigator.userAgent,
        };

        try {
            await this._network.post(`${this._config.apiUrl}/feedback`, temizVeri);
            this._emitter.emit(EVENTS.FEEDBACK_SUBMIT, temizVeri);
            return true;
        } catch (hata) {
            this._emitter.emit(EVENTS.ERROR, { tur: ERROR_TYPES.NETWORK, mesaj: hata.message });
            return false;
        }
    }

    /**
     * Hızlı beğeni/beğenmeme
     * @param {string}          url
     * @param {"up"|"down"}     puan
     * @returns {Promise<boolean>}
     */
    async hizliOy(url, puan) {
        return this.gonder({ url, tur: "hizli_oy", mesaj: puan, rating: puan === "up" ? 5 : 1 });
    }
}

// =============================================================================
// BÖLÜM 23 — OKUMA MODU (ReadingModeManager)
// =============================================================================

class DocsReadingMode {
    /**
     * @param {DocsEventEmitter} emitter
     */
    constructor(emitter) {
        this._emitter = emitter;
        this._etkin = false;
        this._oncekiStil = null;
    }

    /**
     * Okuma modunu aç/kapat
     */
    toggle() {
        this._etkin ? this.kapat() : this.ac();
    }

    /**
     * Okuma modunu aç
     */
    ac() {
        this._etkin = true;
        DOM.sinif.ekle(document.documentElement, "docs-reading-mode");
        this._emitter.emit("docs:reading:open");
        DocsA11y.duyur("Okuma modu açıldı.");
    }

    /**
     * Okuma modunu kapat
     */
    kapat() {
        this._etkin = false;
        DOM.sinif.kaldir(document.documentElement, "docs-reading-mode");
        this._emitter.emit("docs:reading:close");
        DocsA11y.duyur("Okuma modu kapatıldı.");
    }

    /** Etkin mi */
    get etkin() { return this._etkin; }
}

// =============================================================================
// BÖLÜM 24 — GLOBAL ÖRNEK VE EXPORTS
// =============================================================================

/**
 * Varsayılan docs örneği
 */
const docs = new DocsAPI();

/**
 * openDocumentation — docs bağlamında
 * @param {string} [url]
 */
docs.openDocumentation = docs.openDocumentation.bind(docs);

/**
 * closeDocumentation — docs bağlamında
 */
docs.closeDocumentation = docs.closeDocumentation.bind(docs);

/**
 * initialize — docs bağlamında
 */
docs.initialize = docs.initialize.bind(docs);

/**
 * render — docs bağlamında (requestAnimationFrame döngüsü)
 */
docs.render = docs.render.bind(docs);

// =============================================================================
// BÖLÜM 25 — DOM YÜKLENDİĞİNDE OLAY DİNLEYİCİLERİ
// =============================================================================

document.addEventListener("DOMContentLoaded", () => {
    // Otomatik başlatma
    docs.initialize();

    // Link tıklamaları
    document.documentElement.addEventListener("click", (event) => {
        const hedef = event.target.closest(SELECTORS.LINK);
        if (!hedef) return;
        event.preventDefault();
        const url = hedef.getAttribute("href") ?? hedef.dataset.url;
        if (url) docs.openDocumentation(url);
    });

    // Kapatma tıklaması
    document.documentElement.addEventListener("click", (event) => {
        if (event.target.matches(SELECTORS.CLOSE_BTN)) {
            event.preventDefault();
            docs.closeDocumentation();
        }
    });

    // Body düzeyinde tıklama delegasyonu
    document.body.addEventListener("click", (event) => {
        const link = event.target.closest(SELECTORS.LINK);
        if (link) {
            event.preventDefault();
            const url = link.getAttribute("href");
            if (url) docs.openDocumentation(url);
        }
        if (event.target.matches(SELECTORS.CLOSE_BTN)) {
            event.preventDefault();
            docs.closeDocumentation();
        }
    });
});

// Klavye olayları
document.documentElement.addEventListener("keydown", (event) => {
    if (event.key === "Escape") {
        docs.closeDocumentation();
    } else if (event.key === "F1") {
        event.preventDefault();
        docs.openDocumentation();
    }
});

// =============================================================================
// BÖLÜM 26 — CommonJS / ESM EXPORTS
// =============================================================================

const docsExports = {
    // Ana API
    docs,
    DocsAPI,

    // Alt sistemler
    DocsEventEmitter,
    DocsCache,
    DocsState,
    DocsRenderer,
    DocsSearchEngine,
    DocsNavigator,
    DocsVersionManager,
    DocsThemeManager,
    DocsHotkeyManager,
    DocsNetworkClient,
    DocsPluginSystem,
    DocsTOCManager,
    DocsBookmarkManager,
    DocsFeedbackManager,
    DocsReadingMode,
    DocsLogger,

    // Yardımcılar
    Utils,
    DOM,
    DocsSanitizer,
    DocsExporter,
    DocsA11y,

    // Sabitler
    DOCS_META,
    SELECTORS,
    CSS_CLASSES,
    DEFAULT_CONFIG,
    EVENTS,
    HTTP_STATUS,
    ERROR_TYPES,
    LOG_LEVELS,
};

// CommonJS
if (typeof module !== "undefined" && module.exports) {
    module.exports = docsExports;
}

// Tarayıcı global
if (typeof window !== "undefined") {
    window.Docs = docsExports;
    window.docs = docs;
}

// ESM (bundler için)
if (typeof exports !== "undefined") {
    Object.assign(exports, docsExports);
}