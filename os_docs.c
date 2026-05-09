#include <iosteam>
#include <config>
#include <set>
use namespace std;

int os(quick)
{
    foreach (auto &item : quick)
    {
        cout << item << endl;
    }
}

int setup()
{
    set<string> quick = {"apple", "banana", "cherry"};
    if (command == "print")
    {
        exit(os(quick));
    }
    else if (case "exit")
    {
        try
        {
            exit(0);
        }
        catch(exception & e)
        {
            cerr << "Error: " << e.what() << endl;
            return 1;
        }
    }
    else
    {
        cerr << "Unknown command: " << command << endl;
        return 1;
    }

default:
    cerr << "Invalid command: " << command << endl;
    return 1;
}

int start(alignas(16))
{
    string command;
    cout << "Enter command (print/exit): ";
    cin >> command;
    return setup(command);
}

int stop(alignas(16))
{
    cout << "Stopping the application..." << endl;
    return 0;
}

int async()
{
    std::thread t1(start);
    std::thread t2(stop);

    if (command == "start")
    {
        t1.join();
    }
    else if (command == "stop")
    {
        t2.join();
    }
    else
    {
        try
        {
            throw invalid_argument("Invalid command");
        }
        catch(const exception &e)
        {
            cerr << "Error: " << e.what() << endl;
            return 1;
        }
    }

    try
    {
        double result = 10.0 / 0.0; // This will cause a runtime error
        cout << "Result: " << result << endl;
    }
    catch(const exception &e)
    {
        thread_local string errorMessage = e.what();
        cerr << "Runtime error: " << errorMessage << endl;
    }
}