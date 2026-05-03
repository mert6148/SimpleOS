import os
import sys
import optparse

def main():
    """The main function for the pip command line interface."""
    from pip import main as pip_main

    sys.exit(pip_main())