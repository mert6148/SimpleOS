const name = "@table";

const defaultOptions = {
    striped: true,
    hover: true,
    sortable: true,
    responsive: true,
    selectable: false,
    searchable: false,
    filterableColumns: [],
    pageSize: 0,
    currentPage: 1,
    caption: "",
    noDataMessage: "No data available",
    classes: "",
    rowClass: () => "",
    onRowClick: null,
    onRowSelect: null,
    onSelectionChange: null,
    filter: null,
    theme: "light",
    exportable: false,
    columnResizable: false,
    columnHideable: false,
};

function createElement(tag, attributes = {}, children = []) {
    const element = document.createElement(tag);

    for (const [name, value] of Object.entries(attributes)) {
        if (value == null) {
            continue;
        }

        if (name === "className") {
            element.className = value;
        } else if (name === "dataset") {
            Object.assign(element.dataset, value);
        } else if (name.startsWith("on") && typeof value === "function") {
            element.addEventListener(name.slice(2).toLowerCase(), value);
        } else {
            element.setAttribute(name, String(value));
        }
    }

    const childNodes = Array.isArray(children) ? children : [children];
    childNodes.flat().forEach((child) => {
        if (child === null || child === undefined) {
            return;
        }
        if (child instanceof Node) {
            element.appendChild(child);
        } else {
            element.appendChild(document.createTextNode(String(child)));
        }
    });

    return element;
}

function normalizeColumns(columns) {
    return columns.map((column) => ({
        key: column.key || column.field || "",
        label: column.label || String(column.key || column.field || ""),
        sortable: column.sortable !== false,
        width: column.width || null,
        className: column.className || "",
        render: typeof column.render === "function" ? column.render : null,
    }));
}

function sortData(data, sortState, columns) {
    if (!sortState || !sortState.key) {
        return data.slice();
    }

    const direction = sortState.direction === "desc" ? -1 : 1;
    const column = columns.find((item) => item.key === sortState.key);

    if (!column) {
        return data.slice();
    }

    return data.slice().sort((a, b) => {
        const aValue = a[sortState.key];
        const bValue = b[sortState.key];

        if (aValue === bValue) {
            return 0;
        }

        if (aValue === null || aValue === undefined) {
            return 1 * direction;
        }
        if (bValue === null || bValue === undefined) {
            return -1 * direction;
        }

        if (typeof aValue === "number" && typeof bValue === "number") {
            return (aValue - bValue) * direction;
        }

        return String(aValue).localeCompare(String(bValue), undefined, { numeric: true }) * direction;
    });
}

function filterData(data, filter) {
    if (!filter) {
        return data;
    }

    if (typeof filter === "function") {
        return data.filter(filter);
    }

    const search = String(filter).toLowerCase();
    return data.filter((row) =>
        Object.values(row).some((value) => String(value).toLowerCase().includes(search))
    );
}

function createTableHeader(columns, sortState, onSort, options) {
    const headerCells = [];

    if (options.selectable) {
        const selectAllCheckbox = createElement("input", {
            type: "checkbox",
            className: "table-select-all-checkbox",
            onChange: (e) => {
                const checkboxes = document.querySelectorAll(".table-row-checkbox");
                checkboxes.forEach((checkbox) => {
                    checkbox.checked = e.target.checked;
                    checkbox.dispatchEvent(new Event("change"));
                });
            },
        });
        const th = createElement("th", { scope: "col", className: "table-checkbox-header" }, selectAllCheckbox);
        headerCells.push(th);
    }

    const columnHeaders = columns.filter(col => !col.hidden).map((column) => {
        const hasSort = column.sortable && typeof onSort === "function";
        const th = createElement("th", {
            className: column.className,
            scope: "col",
            style: column.width ? `width: ${column.width};` : null,
            onClick: hasSort ? () => onSort(column.key) : null,
        }, [column.label]);

        if (hasSort) {
            const direction = sortState?.key === column.key ? sortState.direction : null;
            const indicator = direction === "asc" ? " ▲" : direction === "desc" ? " ▼" : "";
            th.appendChild(createElement("span", { className: "table-sort-indicator" }, indicator));
            th.classList.add("sortable");
        }

        return th;
    });

    headerCells.push(...columnHeaders);
    const row = createElement("tr", {}, headerCells);
    return createElement("thead", {}, row);
}

function createTableBody(columns, data, options) {
    if (!data.length) {
        return createElement("tbody", {},
            createElement("tr", {},
                createElement("td", { colSpan: columns.length + (options.selectable ? 1 : 0), className: "table-empty" }, options.noDataMessage)
            )
        );
    }

    const rows = data.map((row, rowIndex) => {
        const tr = createElement("tr", {
            className: [options.rowClass(row, rowIndex), rowIndex % 2 === 1 && options.striped ? "striped" : ""].filter(Boolean).join(" "),
            onClick: typeof options.onRowClick === "function" ? () => options.onRowClick(row, rowIndex) : null,
            dataset: { rowIndex },
        });

        if (options.selectable) {
            const checkbox = createElement("input", {
                type: "checkbox",
                className: "table-row-checkbox",
                dataset: { rowIndex },
                onChange: (e) => {
                    if (typeof options.onRowSelect === "function") {
                        options.onRowSelect(row, e.target.checked, rowIndex);
                    }
                },
            });
            const td = createElement("td", { className: "table-checkbox-cell" }, checkbox);
            tr.appendChild(td);
        }

        const cells = columns.filter(col => !col.hidden).map((column) => {
            const value = row[column.key];
            const content = column.render ? column.render(value, row, rowIndex) : value;
            return createElement("td", { className: column.className }, content);
        });

        cells.forEach((cell) => tr.appendChild(cell));
        return tr;
    });

    return createElement("tbody", {}, rows);
}

function createPagination(totalRows, pageSize, currentPage, onNavigate) {
    if (!pageSize || totalRows <= pageSize) {
        return null;
    }

    const pageCount = Math.max(1, Math.ceil(totalRows / pageSize));
    const buttons = [];

    for (let page = 1; page <= pageCount; page += 1) {
        buttons.push(createElement("button", {
            type: "button",
            className: page === currentPage ? "table-page-button active" : "table-page-button",
            onClick: () => onNavigate(page),
        }, String(page)));
    }

    return createElement("div", { className: "table-pagination" }, buttons);
}

function exportTableData(data, columns, format = "csv") {
    const visibleColumns = columns.filter(col => !col.hidden);

    if (format === "json") {
        return JSON.stringify(data, null, 2);
    }

    if (format === "csv") {
        const headers = visibleColumns.map(col => `"${col.label}"`).join(",");
        const rows = data.map(row =>
            visibleColumns.map(col => {
                const value = String(row[col.key] || "");
                return `"${value.replace(/"/g, '""')}"`;
            }).join(",")
        );
        return [headers, ...rows].join("\n");
    }

    return "";
}

function createSearchBar(onSearch) {
    const container = createElement("div", { className: "table-search-container" });
    const input = createElement("input", {
        type: "text",
        className: "table-search-input",
        placeholder: "Arama...",
        onInput: (e) => onSearch(e.target.value),
    });
    container.appendChild(input);
    return container;
}

function createColumnFilter(columns, onFilterChange) {
    const container = createElement("div", { className: "table-column-filter" });
    const label = createElement("label", {}, "Sütunları göster/gizle: ");
    container.appendChild(label);

    columns.forEach((column) => {
        const checkboxContainer = createElement("div", { className: "table-column-checkbox" });
        const checkbox = createElement("input", {
            type: "checkbox",
            checked: !column.hidden,
            onChange: (e) => onFilterChange(column.key, !e.target.checked),
        });
        const labelText = createElement("label", {}, [checkbox, ` ${column.label}`]);
        checkboxContainer.appendChild(labelText);
        container.appendChild(checkboxContainer);
    });

    return container;
}

function createTableToolbar(options, data, columns, actions) {
    if (!options.exportable && !options.columnHideable && !options.searchable) {
        return null;
    }

    const toolbar = createElement("div", { className: "table-toolbar" });

    if (options.searchable) {
        toolbar.appendChild(createSearchBar(actions.onSearch));
    }

    if (options.columnHideable) {
        toolbar.appendChild(createColumnFilter(columns, actions.onColumnToggle));
    }

    if (options.exportable) {
        const exportContainer = createElement("div", { className: "table-export-buttons" });

        const csvBtn = createElement("button", {
            type: "button",
            className: "table-export-btn",
            onClick: () => {
                const csv = exportTableData(data, columns, "csv");
                downloadFile(csv, "table-data.csv", "text/csv");
            },
        }, "CSV indir");

        const jsonBtn = createElement("button", {
            type: "button",
            className: "table-export-btn",
            onClick: () => {
                const json = exportTableData(data, columns, "json");
                downloadFile(json, "table-data.json", "application/json");
            },
        }, "JSON indir");

        exportContainer.appendChild(csvBtn);
        exportContainer.appendChild(jsonBtn);
        toolbar.appendChild(exportContainer);
    }

    return toolbar;
}

function downloadFile(content, filename, type) {
    const blob = new Blob([content], { type });
    const url = window.URL.createObjectURL(blob);
    const link = createElement("a", {
        href: url,
        download: filename,
        style: "display:none;",
    });
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
    window.URL.revokeObjectURL(url);
}

class Table {
    constructor(container, columns, data, options = {}) {
        const element = typeof container === "string" ? document.querySelector(container) : container;

        if (!(element instanceof Element)) {
            throw new Error("Container element not found.");
        }

        this.container = element;
        this.columns = normalizeColumns(columns);
        this.data = Array.isArray(data) ? data.slice() : [];
        this.options = { ...defaultOptions, ...options };
        this.sortState = null;
        this.currentPage = Number(this.options.currentPage) || 1;
        this.tableElement = null;
        this.selectedRows = new Set();
        this.searchQuery = "";
        this.applyTheme();
        this.render();
    }

    setData(data) {
        this.data = Array.isArray(data) ? data.slice() : [];
        this.currentPage = 1;
        this.selectedRows.clear();
        this.render();
    }

    setColumns(columns) {
        this.columns = normalizeColumns(columns);
        this.render();
    }

    setOptions(options) {
        this.options = { ...this.options, ...options };
        if (options.currentPage !== undefined) {
            this.currentPage = Number(options.currentPage) || 1;
        }
        if (options.theme) {
            this.applyTheme();
        }
        this.render();
    }

    sortBy(key) {
        if (!this.options.sortable) {
            return;
        }

        if (this.sortState?.key === key) {
            this.sortState.direction = this.sortState.direction === "asc" ? "desc" : "asc";
        } else {
            this.sortState = { key, direction: "asc" };
        }

        this.render();
    }

    goToPage(page) {
        const totalPageCount = Math.max(1, Math.ceil(this.filteredData.length / this.options.pageSize));
        this.currentPage = Math.min(Math.max(1, page), totalPageCount);
        this.render();
    }

    toggleColumnVisibility(columnKey, hidden) {
        const column = this.columns.find(col => col.key === columnKey);
        if (column) {
            column.hidden = hidden;
            this.render();
        }
    }

    search(query) {
        this.searchQuery = query.toLowerCase();
        this.currentPage = 1;
        this.render();
    }

    getSelectedRows() {
        return Array.from(this.selectedRows).map(index => this.filteredData[index]);
    }

    clearSelection() {
        this.selectedRows.clear();
        const checkboxes = this.tableElement?.querySelectorAll(".table-row-checkbox");
        if (checkboxes) {
            checkboxes.forEach(checkbox => checkbox.checked = false);
        }
    }

    exportData(format = "csv") {
        return exportTableData(this.data, this.columns, format);
    }

    applyTheme() {
        if (this.options.theme === "dark") {
            this.container.classList.add("table-theme-dark");
        } else {
            this.container.classList.remove("table-theme-dark");
        }
    }

    destroy() {
        if (this.tableElement && this.tableElement.parentNode) {
            this.tableElement.parentNode.removeChild(this.tableElement);
        }
        this.tableElement = null;
    }

    render() {
        const options = this.options;
        let filtered = filterData(this.data, options.filter);

        if (this.searchQuery) {
            filtered = filtered.filter(row =>
                Object.values(row).some(value => String(value).toLowerCase().includes(this.searchQuery))
            );
        }

        const sorted = sortData(filtered, this.sortState, this.columns);
        this.filteredData = sorted;

        let pageData = sorted;
        if (options.pageSize > 0) {
            const start = (this.currentPage - 1) * options.pageSize;
            pageData = sorted.slice(start, start + options.pageSize);
        }

        const wrapper = createElement("div", { className: ["table-wrapper", options.classes, options.responsive ? "table-responsive" : ""].filter(Boolean).join(" ") });

        if (options.caption) {
            wrapper.appendChild(createElement("div", { className: "table-caption" }, options.caption));
        }

        const toolbar = createTableToolbar(options, this.data, this.columns, {
            onSearch: (query) => this.search(query),
            onColumnToggle: (key, hidden) => this.toggleColumnVisibility(key, hidden),
        });
        if (toolbar) {
            wrapper.appendChild(toolbar);
        }

        const table = createElement("table", { className: "table-component" });
        table.appendChild(createTableHeader(this.columns, this.sortState, (key) => this.sortBy(key), options));
        table.appendChild(createTableBody(this.columns, pageData, options));

        wrapper.appendChild(table);

        if (options.pageSize > 0) {
            const pagination = createPagination(sorted.length, options.pageSize, this.currentPage, (page) => this.goToPage(page));
            if (pagination) {
                wrapper.appendChild(pagination);
            }
        }

        if (this.tableElement && this.tableElement.parentNode) {
            this.tableElement.parentNode.replaceChild(wrapper, this.tableElement);
        } else {
            this.container.appendChild(wrapper);
        }

        this.tableElement = wrapper;
    }
}

function renderTable(container, columns, data, options = {}) {
    return new Table(container, columns, data, options);
}

function createTable(container, columns, data, options = {}) {
    return renderTable(container, columns, data, options);
}

export { name, Table, renderTable, createTable, defaultOptions };

export default {
    name,
    Table,
    renderTable,
    createTable,
    defaultOptions,
};

documentation.registerComponent({
    name,
    description: "A table component for displaying tabular data.",
    props: {
        container: {
            type: [String, Element],
            required: true,
            description: "CSS selector or DOM element in which the table is rendered.",
        },
        data: {
            type: Array,
            required: true,
            description: "The data rows to display in the table.",
        },
        columns: {
            type: Array,
            required: true,
            description: "The table columns and their render definitions.",
        },
        options: {
            type: Object,
            required: false,
            description: "Optional table settings like pagination, sorting, and styling.",
        },
    },
});