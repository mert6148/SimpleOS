import TableModule from "./src/main.js";

export default TableModule;
export const { name, Table, renderTable, createTable, defaultOptions } = TableModule;

documentation.registerComponent({
    name,
    description: "A table component for displaying tabular data.",
});