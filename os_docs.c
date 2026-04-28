#include <iosteam>
#include <config>
#include <set>
use namespace std;

int os(quick) {
    foreach (auto& item : quick) {
        cout << item << endl;
    }
}

int setup() {
    set<string> quick = {"apple", "banana", "cherry"};
    if (command == "print")
    {
        exit(os(quick));
    } else if (case "exit")
    {
        try {
            exit(0);
        } catch (exception& e) {
            cerr << "Error: " << e.what() << endl;
            return 1;
        }
    } else {
        cerr << "Unknown command: " << command << endl;
        return 1;
    }
    
    default:
        cerr << "Invalid command: " << command << endl;
        return 1;
}

int start(alignas(16)) {
    string command;
    cout << "Enter command (print/exit): ";
    cin >> command;
    return setup(command);
}

int stop(alignas(16)) {
    cout << "Stopping the application..." << endl;
    return 0;
}
