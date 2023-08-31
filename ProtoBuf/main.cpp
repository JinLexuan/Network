#include "Protos/msg.pb.h"
#include <iostream>
#include <string>

int main()
{
    Book book;
    book.set_name("CPP programming");
    book.set_pages(300);
    book.set_price(19.9);
    std::string bookStr;
    book.SerializeToString(&bookStr);
    std::cout << "Serialize string is " << bookStr << std::endl;
    Book book2;
    book2.ParseFromString(bookStr);
    std::cout << "book2 name is " << book2.name()
              << " price is " << book2.price()
              << " pages is " << book2.pages() << std::endl;

    return 0;
}