package com.googledocumentation.sdk;

public final class HtmlRenderer {
    private HtmlRenderer() {
        // Utility class
    }

    public static String renderMarkdown(String markdown) {
        if (markdown == null || markdown.isBlank()) {
            return "";
        }

        var escaped = markdown
                .replace("&", "&amp;")
                .replace("<", "&lt;")
                .replace(">", "&gt;");

        escaped = escaped.replace("\r\n", "\n").replace("\r", "\n");
        escaped = escaped.replaceAll("(?m)^# (.+)$", "<h1>$1</h1>");
        escaped = escaped.replaceAll("(?m)^## (.+)$", "<h2>$1</h2>");
        escaped = escaped.replaceAll("(?m)^### (.+)$", "<h3>$1</h3>");
        escaped = escaped.replaceAll("(?m)^#### (.+)$", "<h4>$1</h4>");
        escaped = escaped.replaceAll("(?m)^##### (.+)$", "<h5>$1</h5>");
        escaped = escaped.replaceAll("(?m)^###### (.+)$", "<h6>$1</h6>");
        escaped = escaped.replaceAll("\\*\\*(.+?)\\*\\*", "<strong>$1</strong>");
        escaped = escaped.replaceAll("\\*(.+?)\\*", "<em>$1</em>");
        escaped = escaped.replaceAll("`(.+?)`", "<code>$1</code>");

        var html = escaped.replaceAll("\n{2,}", "</p><p>");
        html = "<div class=\"markdown-content\"><p>" + html.trim() + "</p></div>";
        return html.replace("<p></p>", "");
    }
}
