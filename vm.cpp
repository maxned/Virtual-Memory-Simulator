//
//  main.cpp
//  Virtual Memory
//
//  Created by Max Nedorezov on 12/7/17.
//  Copyright © 2017 Max Nedorezov. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdint.h>

using namespace std;

struct Page
{
    uint32_t address;
    bool used;

    Page()
    {
        address = 0;
        used = false;
    }

    Page *next;
    bool first; // used so when printing the output, we will always start from here
};

Page* addEntryToPageTable(uint32_t address, Page *page);
Page* makePageTable();
void printOutputToFile(Page *current, ofstream &outputFile);

int main(int argc, char **argv)
{
    string fileName;
    fileName = argv[1];

    ifstream inputFile(fileName.c_str());
    inputFile >> hex;

    ofstream outputFile;
    outputFile.open("vm-out.txt");

    Page *current = makePageTable();

    uint32_t address = 0;
    for (uint32_t i = 0; i < 32; i++)
        inputFile >> address; // ignore first 32 entries

    while (inputFile >> address)
    {
        address = (address & 0xfffffc00);
        current = addEntryToPageTable(address, current);
        printOutputToFile(current, outputFile);
    }

    inputFile.close();
    outputFile.close();

    return 0;
}

Page* addEntryToPageTable(uint32_t address, Page *page)
{
    Page *current = page;

    bool pageAlreadyInTable = false;

    // check if page is already in the page table and come back to the same spot
    for (int i = 0; i < 4; i++) {
        if (address == current->address) {
            pageAlreadyInTable = true;
            current->used = true;
        }

        current = current->next;
    }

    while (1 && !pageAlreadyInTable)
    {
        if (!current->used) {
            current->address = address;
            current->used = true;
            break;
        } else {
            current->used = false;
            current = current->next;
        }
    }

    if (!pageAlreadyInTable)
        current = current->next; // where to start looking next for the replacement policy

    return current;
}

Page* makePageTable()
{
    Page *firstEntry = new Page;
    Page *secondEntry = new Page;
    Page *thirdEntry = new Page;
    Page *fourthEntry = new Page;

    firstEntry->first = true;
    secondEntry->first = false;
    thirdEntry->first = false;
    fourthEntry->first = false;

    firstEntry->next = secondEntry;
    secondEntry->next = thirdEntry;
    thirdEntry->next = fourthEntry;
    fourthEntry->next = firstEntry;

    return firstEntry;
}

void printOutputToFile(Page *current, ofstream &outputFile)
{
    Page *head = current;

    // find the first entry to start printing to output file
    while (!head->first)
        head = head->next;

    for (uint32_t i = 0; i < 4; i++)
    {
        if (head->address > 0) // make sure the address is valid (this is done for the first few entries)
        {
            if (i != 0) outputFile << " ";
            outputFile << hex << head->address;
        }

        if (i == 3) outputFile << endl;

        head = head->next;
    }
}
