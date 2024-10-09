#include <string.h>
#define REACT_IMPLEMENTATION
#include "react.h"

void req(const char *url, int url_len)
{
    static int count = 0;
    count++;
    html_base_template("lol") {
        h1("style='color: red'") {
            textf("requete num %d au : %.*s", count, url_len, url);
        }
    }
}

void caca()
{
    html_base_template("lol") {
        range(0, 5) {
            h1("style='color: red'") {
                text("caca");
            }
        }
    }
}

void page_404()
{
    html_base_template("lol") {
        h1("style='color: red'") {
            text("Page non trouvée");
        }
    }
}

int main()
{
    open_server(6969);

    while (1) {
        accept_connection();
        read_request();

        char url[50];
        int url_len = get_path(url, 50);
        if (url[url_len-1] == '/')
            url_len--;

        http_request_code(200);
        if (url_match(url, "/req")) {
            req(url, url_len);
        }
        else if (url_match(url, "/caca")) {
            caca();
        }
        else {
            http_request_code(404);
            page_404();
        }

        write_request();
        close_connection();
    }

    close_server();
    return 0;
}
