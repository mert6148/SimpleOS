package com.googledocumentation.sdk;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;

public final class DocumentService {
    private final List<Document> documents = new ArrayList<>();

    public Document createDocument(String title, String content) {
        var document = new Document(title, content);
        documents.add(document);
        return document;
    }

    public Optional<Document> getDocumentById(String id) {
        return documents.stream().filter(document -> document.getId().equals(id)).findFirst();
    }

    public List<Document> getDocuments() {
        return Collections.unmodifiableList(documents);
    }

    public boolean updateDocument(String id, String title, String content) {
        return getDocumentById(id)
                .map(document -> {
                    document.update(title, content);
                    return true;
                })
                .orElse(false);
    }

    public boolean deleteDocument(String id) {
        return getDocumentById(id).map(documents::remove).orElse(false);
    }

    public List<Document> search(String query) {
        if (query == null || query.isBlank()) {
            return getDocuments();
        }

        var normalized = query.trim().toLowerCase();
        return documents.stream()
                .filter(document -> document.getTitle().toLowerCase().contains(normalized)
                        || document.getContent().toLowerCase().contains(normalized))
                .collect(Collectors.toList());
    }
}
