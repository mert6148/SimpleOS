package com.googledocumentation.sdk;

import java.time.Instant;
import java.util.UUID;

public final class Document {
    private final String id;
    private String title;
    private String content;
    private final Instant createdAt;
    private Instant updatedAt;

    public Document(String title, String content) {
        this.id = UUID.randomUUID().toString();
        this.title = title == null || title.isBlank() ? "Untitled Document" : title.trim();
        this.content = content == null ? "" : content.trim();
        this.createdAt = Instant.now();
        this.updatedAt = this.createdAt;
    }

    public String getId() {
        return id;
    }

    public String getTitle() {
        return title;
    }

    public String getContent() {
        return content;
    }

    public Instant getCreatedAt() {
        return createdAt;
    }

    public Instant getUpdatedAt() {
        return updatedAt;
    }

    public void update(String title, String content) {
        if (title != null && !title.isBlank()) {
            this.title = title.trim();
        }
        if (content != null) {
            this.content = content.trim();
        }
        this.updatedAt = Instant.now();
    }

    public String toMarkdown() {
        return "# " + title + "\n\n" + content + "\n";
    }

    public String toHtml() {
        return HtmlRenderer.renderMarkdown(toMarkdown());
    }
}
