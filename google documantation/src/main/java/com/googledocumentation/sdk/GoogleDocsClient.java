package com.googledocumentation.sdk;

import java.util.List;

public final class GoogleDocsClient {
    private final DocumentService service;

    public GoogleDocsClient(DocumentService service) {
        this.service = service;
    }

    public Document createDocument(String title, String content) {
        return service.createDocument(title, content);
    }

    public boolean updateDocument(String id, String title, String content) {
        return service.updateDocument(id, title, content);
    }

    public boolean deleteDocument(String id) {
        return service.deleteDocument(id);
    }

    public Document getDocumentById(String id) {
        return service.getDocumentById(id).orElse(null);
    }

    public List<Document> searchDocuments(String query) {
        return service.search(query);
    }

    public String previewDocumentAsHtml(String id) {
        var document = getDocumentById(id);
        return document == null ? "<p>Document not found.</p>" : document.toHtml();
    }
}
