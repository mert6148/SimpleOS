/**
 * ChromiumDocs — script.js
 * Tam özellikli Google Docs benzeri editör motoru
 */

'use strict';

/* ============================================================
   DURUM (State)
   ============================================================ */
const Durum = {
  zoom:          100,
  kaydedildi:    true,
  kaydTimeri:    null,
  kelimeSayisi:  0,
  karakterSayisi:0,
  satirSayisi:   1,
  sayfaSayisi:   1,
  aktifSekme:    'anahat',   // 'anahat' | 'istatistik'
  sidebarAcik:   true,
  bulPanelAcik:  false,
  aramaMetni:    '',
  sonucIndeks:   0,
  sonuclar:      [],
  gecmis:        [],         // geri al yığıtı (basit)
  vurgulananBaslik: null,
};

/* ============================================================
   DOM REFERANSLARI
   ============================================================ */
const $ = id => document.getElementById(id);
const dom = {
  app:          $('app'),
  topbar:       $('topbar'),
  docTitle:     $('docTitle'),
  saveInd:      $('saveIndicator'),
  saveProgress: $('saveProgress'),
  menubar:      $('menubar'),
  toolbar:      $('toolbar'),
  sidebar:      $('sidebar'),
  pageArea:     $('pageArea'),
  pageWrapper:  $('pageWrapper'),
  editor:       $('editor'),
  statusbar:    $('statusbar'),
  wordCount:    $('wordCount'),
  charCount:    $('charCount'),
  pageCount:    $('pageCount'),
  zoomVal:      $('zoomVal'),
  findPanel:    $('findPanel'),
  findInput:    $('findInput'),
  findCount:    $('findCount'),
  outlineList:  $('outlineList'),
  statsPanel:   $('statsPanel'),
  fontFamily:   $('fontFamily'),
  fontSize:     $('fontSize'),
};

/* ============================================================
   ARAÇ ÇUBUĞU KOMUTLARI
   ============================================================ */
function fmt(cmd, deger = null) {
  dom.editor.focus();
  document.execCommand(cmd, false, deger);
  toolbarGuncelle();
  icerikDegisti();
}

function toolbarGuncelle() {
  const komutlar = {
    bold:          'boldBtn',
    italic:        'italicBtn',
    underline:     'underlineBtn',
    strikeThrough: 'strikeBtn',
    insertOrderedList:   'olBtn',
    insertUnorderedList: 'ulBtn',
    justifyLeft:   'alignLeftBtn',
    justifyCenter: 'alignCenterBtn',
    justifyRight:  'alignRightBtn',
    justifyFull:   'alignJustifyBtn',
    superscript:   'supBtn',
    subscript:     'subBtn',
  };

  for (const [cmd, btnId] of Object.entries(komutlar)) {
    const btn = $(btnId);
    if (btn) btn.classList.toggle('active', document.queryCommandState(cmd));
  }

  /* Yazı tipi & boyutu yansıt */
  try {
    const secilenYazi = document.queryCommandValue('fontName').replace(/['"]/g, '');
    if (secilenYazi) {
      for (const opt of dom.fontFamily.options) {
        if (opt.value.startsWith(secilenYazi) || secilenYazi.startsWith(opt.value.split(',')[0])) {
          dom.fontFamily.value = opt.value;
          break;
        }
      }
    }
    const boyut = document.queryCommandValue('fontSize');
    // execCommand fontSize 1-7 arası döner; piksel için font elementine bak
    const sel = window.getSelection();
    if (sel && sel.rangeCount > 0) {
      const el = sel.anchorNode?.parentElement;
      if (el) {
        const cs = window.getComputedStyle(el);
        const px = Math.round(parseFloat(cs.fontSize));
        if (px && dom.fontSize) {
          const yakın = [...dom.fontSize.options].reduce((a, b) =>
            Math.abs(parseInt(b.value) - px) < Math.abs(parseInt(a.value) - px) ? b : a
          );
          dom.fontSize.value = yakın.value;
        }
      }
    }
  } catch (_) {}
}

/* Yazı tipi değiştirici */
function yazıTipiDegistir(deger) {
  dom.editor.focus();
  document.execCommand('fontName', false, deger);
  icerikDegisti();
}

/* Yazı boyutu değiştirici */
function yazıBoyutuDegistir(px) {
  dom.editor.focus();
  // Önce sahte fontSize 7 uygula, sonra CSS px'e çevir
  document.execCommand('fontSize', false, '7');
  dom.editor.querySelectorAll('font[size="7"]').forEach(el => {
    el.removeAttribute('size');
    el.style.fontSize = px + 'px';
    el.style.lineHeight = '1.15';
  });
  icerikDegisti();
}

/* Metin rengi */
function renkUygula(renk) {
  const dot = $('colorBar');
  if (dot) dot.style.background = renk;
  dom.editor.focus();
  document.execCommand('foreColor', false, renk);
  icerikDegisti();
}

/* Vurgu rengi */
function vurguRenkiUygula(renk) {
  const dot = $('hlColorBar');
  if (dot) dot.style.background = renk;
  dom.editor.focus();
  document.execCommand('hiliteColor', false, renk);
  icerikDegisti();
}

/* Başlık stili */
function baslikUygula(seviye) {
  dom.editor.focus();
  if (!seviye) {
    document.execCommand('formatBlock', false, 'p');
  } else {
    document.execCommand('formatBlock', false, seviye);
  }
  $('headingSelect').value = '';
  anahatGuncelle();
  icerikDegisti();
}

/* Girinti */
function girintiArtir() { fmt('indent'); }
function girintiAzalt() { fmt('outdent'); }

/* Yatay çizgi */
function yatayCizgiEkle() {
  fmt('insertHorizontalRule');
}

/* Bağlantı ekle */
function baglantiEkle() {
  const url = prompt('URL girin:', 'https://');
  if (url) {
    const metin = window.getSelection().toString() || url;
    fmt('createLink', url);
  }
}

/* Tablo ekle */
function tabloEkle(satirSayisi = 3, sutunSayisi = 3) {
  let html = '<table><tbody>';
  for (let r = 0; r < satirSayisi; r++) {
    html += '<tr>';
    for (let c = 0; c < sutunSayisi; c++) {
      html += r === 0 ? '<th>&nbsp;</th>' : '<td>&nbsp;</td>';
    }
    html += '</tr>';
  }
  html += '</tbody></table><p><br></p>';
  fmt('insertHTML', html);
}

/* ============================================================
   ZOOM
   ============================================================ */
function zoomAyarla(yeni) {
  Durum.zoom = Math.max(50, Math.min(200, yeni));
  dom.pageWrapper.style.transform = `scale(${Durum.zoom / 100})`;
  if (dom.zoomVal) dom.zoomVal.textContent = Durum.zoom + '%';
  if ($('zoomLevel')) $('zoomLevel').textContent = '%' + Durum.zoom;
}

function zoomArtir() { zoomAyarla(Durum.zoom + 10); }
function zoomAzalt() { zoomAyarla(Durum.zoom - 10); }

/* ============================================================
   KAYDETME SİMÜLASYONU
   ============================================================ */
function kaydetmeBaslat() {
  Durum.kaydedildi = false;
  dom.saveInd.textContent = 'Kaydediliyor...';

  // İlerleme çubuğu
  dom.saveProgress.className = 'saving';

  clearTimeout(Durum.kaydTimeri);
  Durum.kaydTimeri = setTimeout(() => {
    dom.saveInd.textContent = 'Tüm değişiklikler Drive\'a kaydedildi';
    Durum.kaydedildi = true;
    dom.saveProgress.className = 'done';
    setTimeout(() => { dom.saveProgress.className = 'fade'; }, 400);
    setTimeout(() => { dom.saveProgress.className = ''; }, 900);
  }, 1400);
}

/* ============================================================
   İÇERİK DEĞİŞİKLİĞİ
   ============================================================ */
function icerikDegisti() {
  kaydetmeBaslat();
  istatistikGuncelle();
  anahatGuncelle();
}

/* ============================================================
   İSTATİSTİK
   ============================================================ */
function istatistikGuncelle() {
  const metin = dom.editor.innerText || '';
  const temizMetin = metin.trim();

  const kelimeler = temizMetin ? temizMetin.split(/\s+/).filter(Boolean).length : 0;
  const karakterler = temizMetin.replace(/\s/g, '').length;
  const satirlar = metin.split('\n').filter(s => s.trim()).length;

  Durum.kelimeSayisi  = kelimeler;
  Durum.karakterSayisi = karakterler;
  Durum.satirSayisi   = satirlar;

  if (dom.wordCount)  dom.wordCount.textContent  = kelimeler + ' kelime';
  if (dom.charCount)  dom.charCount.textContent  = karakterler + ' karakter';

  // Sayfa tahmini (yaklaşık 500 kelime/sayfa)
  const sayfaTahmini = Math.max(1, Math.ceil(kelimeler / 500));
  Durum.sayfaSayisi = sayfaTahmini;
  if (dom.pageCount) dom.pageCount.textContent = 'Sayfa 1/' + sayfaTahmini;

  // İstatistik paneli
  const panel = $('statsContent');
  if (panel && Durum.aktifSekme === 'istatistik') {
    panel.innerHTML = `
      <div class="word-panel">
        <div class="word-panel-label">Kelime sayısı</div>
        <div class="word-panel-stat">${kelimeler.toLocaleString('tr')}</div>
        <div class="word-panel-sub">yaklaşık ${sayfaTahmini} sayfa</div>
      </div>
      <div style="border-top:1px solid #e8eaed; margin: 0 16px;"></div>
      <div style="padding: 8px 16px;">
        <div class="word-panel-row"><span>Karakter (boşluksuz)</span><span class="word-panel-num">${karakterler.toLocaleString('tr')}</span></div>
        <div class="word-panel-row"><span>Karakter (boşluklu)</span><span class="word-panel-num">${temizMetin.length.toLocaleString('tr')}</span></div>
        <div class="word-panel-row"><span>Paragraf/Satır</span><span class="word-panel-num">${satirlar}</span></div>
        <div class="word-panel-row"><span>Başlık sayısı</span><span class="word-panel-num">${dom.editor.querySelectorAll('h1,h2,h3,h4').length}</span></div>
        <div class="word-panel-row"><span>Okuma süresi</span><span class="word-panel-num">≈ ${Math.max(1, Math.round(kelimeler / 200))} dk</span></div>
      </div>
    `;
  }
}

/* ============================================================
   ANAHAT (Outline)
   ============================================================ */
function anahatGuncelle() {
  const liste = dom.outlineList;
  if (!liste) return;

  const basliklar = dom.editor.querySelectorAll('h1, h2, h3, h4');
  if (basliklar.length === 0) {
    liste.innerHTML = '<div style="padding:12px 16px;font-size:12px;color:#9aa0a6;">Henüz başlık yok.<br>Başlık eklemek için araç çubuğundaki başlık stillerini kullanın.</div>';
    return;
  }

  liste.innerHTML = '';
  basliklar.forEach((el, i) => {
    if (!el.id) el.id = 'baslik-' + i;
    const seviye = el.tagName.toLowerCase(); // h1, h2, h3
    const div = document.createElement('div');
    div.className = `outline-item ${seviye}`;
    div.textContent = el.textContent.trim() || '(boş başlık)';
    div.onclick = () => {
      el.scrollIntoView({ behavior: 'smooth', block: 'start' });
      liste.querySelectorAll('.outline-item').forEach(d => d.classList.remove('active'));
      div.classList.add('active');
    };
    liste.appendChild(div);
  });
}

/* ============================================================
   KENAR ÇUBUĞU SEKMELERİ
   ============================================================ */
function sekmeAc(sekme) {
  Durum.aktifSekme = sekme;
  document.querySelectorAll('.sidebar-tab').forEach(t => {
    t.classList.toggle('active', t.dataset.sekme === sekme);
  });

  const anahatPanel = $('anahatPanel');
  const statsPanel  = $('statsPanel');
  if (anahatPanel) anahatPanel.style.display = sekme === 'anahat' ? '' : 'none';
  if (statsPanel)  statsPanel.style.display  = sekme === 'istatistik' ? '' : 'none';

  if (sekme === 'istatistik') istatistikGuncelle();
}

/* ============================================================
   KENAR ÇUBUĞU AÇ/KAPAT
   ============================================================ */
function sidebarToggle() {
  Durum.sidebarAcik = !Durum.sidebarAcik;
  dom.sidebar.classList.toggle('collapsed', !Durum.sidebarAcik);
}

/* ============================================================
   BUL & DEĞİŞTİR
   ============================================================ */
function bulPanelToggle() {
  Durum.bulPanelAcik = !Durum.bulPanelAcik;
  dom.findPanel.classList.toggle('visible', Durum.bulPanelAcik);
  if (Durum.bulPanelAcik) {
    dom.findInput.focus();
    dom.findInput.select();
  } else {
    vurgulariTemizle();
  }
}

function vurgulariTemizle() {
  dom.editor.querySelectorAll('.arama-vurgu').forEach(el => {
    el.replaceWith(document.createTextNode(el.textContent));
  });
  dom.editor.normalize();
  Durum.sonuclar = [];
  Durum.sonucIndeks = 0;
  if (dom.findCount) dom.findCount.textContent = '';
}

function aramaYap(metin) {
  vurgulariTemizle();
  if (!metin) return;

  // innerHTML üzerinde değil TreeWalker ile metin düğümlerinde ara
  const walker = document.createTreeWalker(dom.editor, NodeFilter.SHOW_TEXT);
  const eslesmeler = [];
  let node;
  while ((node = walker.nextNode())) {
    const idx = node.textContent.toLowerCase().indexOf(metin.toLowerCase());
    if (idx !== -1) eslesmeler.push({ node, idx, uzunluk: metin.length });
  }

  // Geriye doğru işle (offset korunması için)
  for (let i = eslesmeler.length - 1; i >= 0; i--) {
    const { node, idx, uzunluk } = eslesmeler[i];
    const aralik = document.createRange();
    aralik.setStart(node, idx);
    aralik.setEnd(node, idx + uzunluk);
    const span = document.createElement('span');
    span.className = 'arama-vurgu';
    span.style.cssText = 'background:#fdff7f;color:#000;border-radius:2px;';
    aralik.surroundContents(span);
  }

  Durum.sonuclar = dom.editor.querySelectorAll('.arama-vurgu');
  Durum.sonucIndeks = 0;

  if (dom.findCount) {
    dom.findCount.textContent = Durum.sonuclar.length
      ? `${Durum.sonuclar.length} sonuç`
      : 'Sonuç yok';
  }

  aramaAtla(0);
}

function aramaAtla(yon) {
  if (!Durum.sonuclar.length) return;
  const onceki = Durum.sonuclar[Durum.sonucIndeks];
  if (onceki) onceki.style.background = '#fdff7f';

  Durum.sonucIndeks = (Durum.sonucIndeks + yon + Durum.sonuclar.length) % Durum.sonuclar.length;
  const aktif = Durum.sonuclar[Durum.sonucIndeks];
  if (aktif) {
    aktif.style.background = '#ff9900';
    aktif.scrollIntoView({ behavior: 'smooth', block: 'center' });
  }
}

/* ============================================================
   MENÜ DROPDOWN SİSTEMİ
   ============================================================ */
let acikMenu = null;

function menuAc(btnEl, menüId) {
  const menu = $(menüId);
  if (!menu) return;

  if (acikMenu && acikMenu !== menu) {
    acikMenu.classList.remove('visible');
    document.querySelectorAll('.menu-btn').forEach(b => b.classList.remove('open'));
  }

  const acik = menu.classList.toggle('visible');
  btnEl.classList.toggle('open', acik);

  if (acik) {
    acikMenu = menu;
    const rect = btnEl.getBoundingClientRect();
    menu.style.left = rect.left + 'px';
  } else {
    acikMenu = null;
  }
}

document.addEventListener('click', e => {
  if (acikMenu && !e.target.closest('.menu-btn') && !e.target.closest('.menu-dropdown')) {
    acikMenu.classList.remove('visible');
    document.querySelectorAll('.menu-btn').forEach(b => b.classList.remove('open'));
    acikMenu = null;
  }
});

/* ============================================================
   YAZDIRMA
   ============================================================ */
function yazdir() {
  window.print();
}

/* ============================================================
   DIŞA AKTAR (HTML indirme)
   ============================================================ */
function htmlOlarakIndir() {
  const baslik = dom.docTitle.value || 'belge';
  const icerik = dom.editor.innerHTML;
  const html = `<!DOCTYPE html>
<html lang="tr">
<head>
  <meta charset="UTF-8">
  <title>${baslik}</title>
  <style>
    body { font-family: Georgia, serif; font-size: 11pt; line-height: 1.15; max-width: 816px; margin: 96px auto; color: #000; }
    h1 { font-family: sans-serif; font-size: 20pt; font-weight: 400; }
    h2 { font-family: sans-serif; font-size: 15pt; font-weight: 500; color: #434343; }
    h3 { font-family: sans-serif; font-size: 11pt; font-weight: 500; color: #666; }
    table { width: 100%; border-collapse: collapse; }
    th, td { border: 1px solid #dadce0; padding: 6pt 8pt; }
    th { background: #f1f3f4; }
    code { font-family: monospace; background: #f1f3f4; padding: 1px 4px; }
  </style>
</head>
<body>${icerik}</body>
</html>`;

  const blob = new Blob([html], { type: 'text/html;charset=utf-8' });
  const a = document.createElement('a');
  a.href = URL.createObjectURL(blob);
  a.download = baslik.replace(/\s+/g, '_') + '.html';
  a.click();
  URL.revokeObjectURL(a.href);
}

/* ============================================================
   KLAVYE KISAYOLLARI
   ============================================================ */
document.addEventListener('keydown', e => {
  const ctrl = e.ctrlKey || e.metaKey;

  if (ctrl) {
    switch (e.key.toLowerCase()) {
      case 'b': e.preventDefault(); fmt('bold'); break;
      case 'i': e.preventDefault(); fmt('italic'); break;
      case 'u': e.preventDefault(); fmt('underline'); break;
      case 'f': e.preventDefault(); bulPanelToggle(); break;
      case 'z': e.preventDefault(); fmt(e.shiftKey ? 'redo' : 'undo'); break;
      case 'y': e.preventDefault(); fmt('redo'); break;
      case 'p': e.preventDefault(); yazdir(); break;
      case 's': e.preventDefault(); kaydetmeBaslat(); break;
      case '\\': e.preventDefault(); sidebarToggle(); break;
    }
    if (e.key === '=' && !e.shiftKey)  { e.preventDefault(); fmt('superscript'); }
    if (e.key === '-' && !e.shiftKey)  { e.preventDefault(); fmt('subscript'); }
  }

  // Arama panelinde Enter / Escape
  if (e.target === dom.findInput) {
    if (e.key === 'Enter')  aramaAtla(e.shiftKey ? -1 : 1);
    if (e.key === 'Escape') bulPanelToggle();
  }
});

/* ============================================================
   EDİTÖR OLAYLARI
   ============================================================ */
dom.editor.addEventListener('input', icerikDegisti);
dom.editor.addEventListener('mouseup', toolbarGuncelle);
dom.editor.addEventListener('keyup', e => {
  toolbarGuncelle();
  // Ok tuşlarında da anahat güncellensin
  if (['ArrowUp','ArrowDown','Home','End'].includes(e.key)) anahatGuncelle();
});

// Sayfa tıklaması — anahat aktif öğesi güncelle
dom.editor.addEventListener('click', () => {
  toolbarGuncelle();
  const sel = window.getSelection();
  if (!sel || !sel.rangeCount) return;
  let el = sel.anchorNode?.parentElement;
  while (el && el !== dom.editor) {
    if (/^H[1-4]$/.test(el.tagName) && el.id) {
      dom.outlineList?.querySelectorAll('.outline-item').forEach(item => {
        item.classList.toggle('active', item.onclick?.toString().includes(el.id));
      });
      break;
    }
    el = el.parentElement;
  }
});

/* ============================================================
   BAŞLIK / BELGE ADI
   ============================================================ */
dom.docTitle.addEventListener('input', () => {
  document.title = dom.docTitle.value + ' — ChromiumDocs';
  kaydetmeBaslat();
});

/* ============================================================
   ARAMA KUTUSU
   ============================================================ */
if (dom.findInput) {
  dom.findInput.addEventListener('input', e => aramaYap(e.target.value));
}

/* ============================================================
   ZOOM KAYDIRICI
   ============================================================ */
const zoomSlider = $('zoomSlider');
if (zoomSlider) {
  zoomSlider.addEventListener('input', e => zoomAyarla(parseInt(e.target.value)));
}

/* ============================================================
   BAŞLANGIÇ
   ============================================================ */
document.addEventListener('DOMContentLoaded', () => {
  istatistikGuncelle();
  anahatGuncelle();
  toolbarGuncelle();
  zoomAyarla(100);
  dom.docTitle && (document.title = dom.docTitle.value + ' — ChromiumDocs');
});