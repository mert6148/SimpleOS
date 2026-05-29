package com.googledocumentation.sdk;

public final class App {
    public static void main(String[] args) {
        var service = new DocumentService();
        var client = new GoogleDocsClient(service);

        var intro = client.createDocument("Introduction", "Welcome to the Google Documentation Java SDK.");
        client.createDocument("Quick Start", "Create, update and export documentation with a simple Java service.");

        System.out.println("Documents created:");
        service.getDocuments().forEach(document -> System.out.println("- " + document.getTitle()));

        service.updateDocument(intro.getId(), "Introduction", "This SDK manages documentation content with a simple Java API.");

        System.out.println();
        System.out.println("HTML preview for the first document:");
        System.out.println(client.previewDocumentAsHtml(intro.getId()));
    }
}
