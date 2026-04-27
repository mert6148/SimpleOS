using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GoogleDocumentation
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Welcome to Google Documentation!");
        }

        public static void PrintDocumentation()
        {
            Console.WriteLine("This is the Google Documentation SDK.");
            Console.WriteLine("Here you can find all the necessary information to get started with our SDK.");
        }

        public static void GetStarted()
        {
            Console.WriteLine("To get started, please follow the instructions in the documentation.");
            Console.WriteLine("You can find the documentation at: https://developers.google.com/docs");
        }
    }
}