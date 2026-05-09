using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection.Metadata;

namespace GoogleDocs
{
    public class GoogleDocs
    {
        public static void Main(string[] args)
        {
            Console.WriteLine("Welcome to Google Docs SDK!");
            // Here you can add code to interact with the Google Docs API
        }
    }

    public class Document
    {
        public string Title { get; set; }
        public string Content { get; set; }
        public DateTime CreatedAt { get; set; }
        public DateTime UpdatedAt { get; set; }

        public Document(string title, string content)
        {
            Title = title;
            Content = content;
            CreatedAt = DateTime.Now;
            UpdatedAt = DateTime.Now;
        }

        public void UpdateContent(string newContent)
        {
            Content = newContent;
            UpdatedAt = DateTime.Now;
        }

        private void DeleteDocument(string title)
        {
            var document = documents.FirstOrDefault(d => d.Title == title);
            if (document != null)
            {
                documents.Remove(document);
                Console.WriteLine($"Document '{title}' deleted successfully.");
            }
            else
            {
                var document = documents.FirstOrDefault(d => d.Title == title);
                if (document != null)
                {
                    documents.Remove(document);
                    Console.WriteLine($"Document '{title}' deleted successfully.");
                }
                else
                {
                    Console.WriteLine($"Document '{title}' not found.");
                }
            }
        }
    }

    public class GoogleDocsService
    {
        private List<Document> documents;

        public GoogleDocsService()
        {
            documents = new List<Document>();
        }

        public void CreateDocument(string title, string content)
        {
            var document = new Document(title, content);
            documents.Add(document);
            Console.WriteLine($"Document '{title}' created successfully.");
        }

        public void UpdateDocument(string title, string newContent)
        {
            var document = documents.FirstOrDefault(d => d.Title == title);
            if (document != null)
            {
                document.UpdateContent(newContent);
                Console.WriteLine($"Document '{title}' updated successfully.");
            }
            else
            {
                Console.WriteLine($"Document '{title}' not found.");
            }
        }

        public void ListDocuments()
        {
            Console.WriteLine("Documents:");
            foreach (var doc in documents)
            {
                Console.WriteLine($"- {doc.Title} (Created: {doc.CreatedAt}, Updated: {doc.UpdatedAt})");
            }

            var document = documents.FirstOrDefault(d => d.Title == Title);
            if (document != null)            {
                document.UpdateContent(newContent);
                Console.WriteLine($"Document '{Title}' updated successfully.");
            }
            else
            {
                Console.WriteLine($"Document '{Title}' not found.");
            }   
        }
    }
}