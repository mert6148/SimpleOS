# TableDB - Minimal C++ Table Database

This folder contains a small in-memory table database implemented in C++.

Features:
- Create/drop tables with named columns
- Insert rows and list/select data
- Save a table to CSV and load CSV files as tables
- Interactive CLI `table_cli`

Build (if you have g++/make):

```bash
cd include/@table
make
```

Run CLI:

```bash
./table_cli
# then use commands like:
# create users id name email
# insert users 1 Alice alice@example.com
# select users
# save users users.csv
# load users.csv
```

Notes:
- The CSV loader derives the table name from the filename (strip extension).
- The CSV format supports quoted fields and doubled quotes escaping.
# SimpleOS Table Component

Güçlü, otomasyonlu ve responsive bir JavaScript tablo bileşeni.

## 🚀 Özellikler

- **Satır Seçimi**: Checkbox ile satır seçme ve toplu işlem
- **Sıralama**: Sütun başlıklarına tıklayarak sırala
- **Sayfalama**: Büyük veri setlerini sayfalara böl
- **Arama**: Gerçek zamanlı metin araması
- **Sütun Filtreleme**: İstedikleri sütunları göster/gizle
- **Dışa Aktarma**: CSV ve JSON formatında veri indir
- **Tema Desteği**: Açık ve koyu tema seçeneği
- **Responsive**: Tüm cihazlarda mükemmel çalışır
- **Animasyonlar**: Yükseltilmiş kullanıcı deneyimi
- **Özel Render**: Hücreleri özel fonksiyonlarla render et

## 📦 Kurulum

### CSS Dosyalarını Dahil Et

```html
<link rel="stylesheet" href="./styles/table.css">
<link rel="stylesheet" href="./styles/table-dark.css">
<link rel="stylesheet" href="./styles/table-animations.css">
```

### JavaScript'i İçe Aktar

```javascript
import { Table, createTable } from "./include/@table";
```

## 🎯 Basit Kullanım

```javascript
// Veri
const data = [
    { id: 1, name: "Ali", email: "ali@example.com" },
    { id: 2, name: "Ayşe", email: "ayse@example.com" }
];

// Sütunlar
const columns = [
    { key: "id", label: "ID" },
    { key: "name", label: "Ad" },
    { key: "email", label: "E-posta" }
];

// Tablo oluştur
const table = new Table("#table-container", columns, data);
```

## ⚙️ Seçenekler

```javascript
const options = {
    // Görünüm
    striped: true,              // Şeritli satırlar
    hover: true,                // Hover efekti
    responsive: true,           // Responsive tasarım
    theme: "light",             // "light" veya "dark"
    caption: "Tablo Başlığı",   // Tablo başlığı
    classes: "custom-class",    // Özel CSS sınıfı

    // İşlevsellik
    sortable: true,             // Sıralama etkin
    selectable: true,           // Satır seçimi etkin
    searchable: true,           // Arama etkin
    columnHideable: true,       // Sütun gizleme etkin
    exportable: true,           // CSV/JSON dışa aktarma
    columnResizable: false,     // Sütun boyutu değiştirme

    // Sayfalama
    pageSize: 10,               // Sayfa başına satır sayısı (0 = tüm satırlar)
    currentPage: 1,             // Başlangıç sayfası

    // Mesajlar
    noDataMessage: "Veri yok",

    // Olaylar
    onRowClick: (row, index) => console.log(row),
    onRowSelect: (row, selected, index) => console.log(row, selected),
    onSelectionChange: (rows) => console.log(rows),

    // Fonksiyonlar
    filter: null,               // Filtre fonksiyonu
    rowClass: (row, index) => "" // Satır sınıfı fonksiyonu
};

const table = new Table("#container", columns, data, options);
```

## 🎨 Sütun Tanımları

```javascript
const columns = [
    {
        key: "name",                           // Veri anahtarı
        label: "Ad",                           // Görüntülenecek başlık
        sortable: true,                        // Sıralanabilir mi?
        width: "200px",                        // Sütun genişliği
        className: "custom-class",             // CSS sınıfı
        render: (value, row, index) => {       // Özel render fonksiyonu
            return `<strong>${value}</strong>`;
        }
    }
];
```

## 📝 API Metodları

### Tablo Kontrolü

```javascript
// Veri güncelle
table.setData(newData);

// Sütunları güncelle
table.setColumns(newColumns);

// Seçenekleri güncelle
table.setOptions({ pageSize: 20 });

// Sayfaya git
table.goToPage(3);

// Veriye göre sırala
table.sortBy("name");

// Temayı değiştir
table.setOptions({ theme: "dark" });

// Sütun görünürlüğünü değiştir
table.toggleColumnVisibility("email", true); // gizle

// Arama yap
table.search("Ali");

// Seçili satırları al
const selected = table.getSelectedRows();

// Seçimi temizle
table.clearSelection();

// Veriyi dışa aktar
const csv = table.exportData("csv");
const json = table.exportData("json");

// Tabloyu sil
table.destroy();
```

## 🎭 Tema Kullanımı

### Açık Tema (Varsayılan)

```html
<div id="table-container"></div>
```

### Koyu Tema

```javascript
const table = new Table("#table-container", columns, data, {
    theme: "dark"
});
```

Veya manuel olarak:

```html
<div id="table-container" class="table-theme-dark"></div>
```

## 📊 Sütun Özel Render Örnekleri

```javascript
const columns = [
    {
        key: "salary",
        label: "Maaş",
        render: (value) => `₺${value.toLocaleString("tr-TR")}`
    },
    {
        key: "status",
        label: "Durum",
        render: (value) => {
            const colors = { aktif: "green", pasif: "red", bekleme: "orange" };
            return `<span style="color: ${colors[value]}">${value}</span>`;
        }
    },
    {
        key: "actions",
        label: "İşlemler",
        render: (value, row) => `
            <button onclick="edit(${row.id})">Düzenle</button>
            <button onclick="delete(${row.id})">Sil</button>
        `
    }
];
```

## 🔍 Arama ve Filtreleme

### Otomatik Arama (Toolbar)

```javascript
const table = new Table("#container", columns, data, {
    searchable: true  // Arama çubuğu göster
});
```

### Programlı Arama

```javascript
table.search("Ali");
```

### Özel Filtreleme

```javascript
const table = new Table("#container", columns, data, {
    filter: (row) => row.department === "Yazılım"
});

// Daha sonra filtreyi değiştir
table.setOptions({
    filter: (row) => row.salary > 50000
});
```

## 💾 Dışa Aktarma

```javascript
// Toolbar'dan (otomatik)
const table = new Table("#container", columns, data, {
    exportable: true
});

// Programlı olarak
const csvData = table.exportData("csv");
const jsonData = table.exportData("json");

// Dosya olarak indir
const link = document.createElement("a");
link.href = "data:text/csv;charset=utf-8," + csvData;
link.download = "data.csv";
link.click();
```

## 🖥️ Responsive Tasarım

Tablo, otomatik olarak tüm cihazlarda responsive hale gelir:

- **Desktop**: Standart tablo düzeni
- **Tablet**: Az biraz daralan tasarım
- **Mobil**: Kart benzeri düzen (her satır bir kart)

Responsive davranışı devre dışı bırakmak için:

```javascript
const table = new Table("#container", columns, data, {
    responsive: false
});
```

## 🎓 Gelişmiş Örnekler

### Satır Seçim ile Toplu İşlem

```javascript
const table = new Table("#container", columns, data, {
    selectable: true,
    onSelectionChange: (selected) => {
        console.log("Seçili satırlar:", selected);
        if (selected.length > 0) {
            deleteButton.disabled = false;
        }
    }
});

deleteButton.onclick = () => {
    const selected = table.getSelectedRows();
    const ids = selected.map(row => row.id);
    console.log("Silinecek IDs:", ids);
};
```

### Dinamik Veri Güncelleme

```javascript
async function loadData() {
    const response = await fetch("/api/users");
    const data = await response.json();
    table.setData(data);
}

// 30 saniyede bir güncelle
setInterval(loadData, 30000);
```

### Şartlı Hücre Renklendirme

```javascript
const columns = [
    {
        key: "status",
        label: "Durum",
        render: (value, row) => {
            const styles = {
                aktif: "background: #d4edda; color: #155724;",
                pasif: "background: #f8d7da; color: #721c24;",
                bekleme: "background: #fff3cd; color: #856404;"
            };
            return `<span style="${styles[value] || ''}">${value}</span>`;
        }
    }
];
```

## 📁 Dosya Yapısı

```
@table/
├── src/
│   └── main.js              # Ana bileşen kodı
├── styles/
│   ├── table.css            # Temel stiller
│   ├── table-dark.css       # Koyu tema
│   └── table-animations.css # Animasyonlar
├── index.js                 # Export dosyası
├── examples.html            # HTML örnekleri
└── README.md               # Bu dosya
```

## 🌐 Tarayıcı Desteği

- Chrome (tüm sürümler)
- Firefox (tüm sürümler)
- Safari (12+)
- Edge (tüm sürümler)
- IE 11+ (bazı özellikler sınırlı)

## ♿ Erişilebilirlik

- Semantic HTML (thead, tbody, th, td)
- ARIA etiketleri
- Klavye navigasyonu desteği
- Ekran okuyucu uyumluluğu
- Renk kontrastı WCAG AA standartlarına uygun

## 📄 Lisans

SimpleOS Project - Eğitim Amaçlı

## 🤝 Katkılar

Geri bildirim ve geliştirme önerileri hoşlanır!

## 📞 Destek

Daha fazla bilgi için `examples.html` dosyasını açın ve canlı örnekleri inceleyin.
