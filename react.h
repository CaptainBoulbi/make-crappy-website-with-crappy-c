#ifndef REACT_H
#define REACT_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define BALISE \
    X(html)    \
    X(head)    \
    X(body)    \
    X(main)    \
    X(p)       \
    X(ul)      \
    X(li)      \
    X(div)     \
    X(h1)      \
    X(meta)    \
    X(link)    \
    X(script)  \
    X(title)

#define X(balise) \
    int balise##_begin(const char *attr); \
    void balise##_end();
BALISE
#undef X

int html_begin(const char *attr);
void html_end();

#define REAL_BALISE_CALL(balise, attr) for (int ZZ = balise##_begin(attr); ZZ; ZZ = 0, balise##_end())

// NOTE: because we cannot have main() as a html balise we call it html_main()
//       and redirect to main_begin() and main_end(), that's why the balise
//       name are not the same even if the other balise have the same name
// UPDATE: same for link(), it already exist in unistd.h

#define html_main(attr) REAL_BALISE_CALL(main, attr)
#define html_link(attr) REAL_BALISE_CALL(link, attr)

#define html(attr) REAL_BALISE_CALL(html, attr)
#define head(attr) REAL_BALISE_CALL(head, attr)
#define body(attr) REAL_BALISE_CALL(body, attr)
#define p(attr) REAL_BALISE_CALL(p, attr)
#define ul(attr) REAL_BALISE_CALL(ul, attr)
#define li(attr) REAL_BALISE_CALL(li, attr)
#define div(attr) REAL_BALISE_CALL(div, attr)
#define h1(attr) REAL_BALISE_CALL(h1, attr)
#define meta(attr) REAL_BALISE_CALL(meta, attr)
#define title(attr) REAL_BALISE_CALL(title, attr)
#define script(attr) REAL_BALISE_CALL(script, attr)

#ifndef REACT_IMPLEMENTATION
#undef BALISE
#endif

void doctype();

void text(const char *txt);
void textf(const char *format, ...);

#define multi_line_text(first, ...) multi_line_text_imp(first, __VA_ARGS__, NULL)
void multi_line_text_imp(const char *first_txt, ...);

#define html_base_template(...) for (int zz=html_base_template_imp(__VA_ARGS__); zz; body_end(), html_end(), zz=0)
int html_base_template_imp(const char *title);


int open_server(int port);
int accept_connection();
int read_request();
int write_request();
void close_connection();
void close_server();

void http_request_code();
void print_html();
int save_html(const char *file_name);

int get_path(char *buf, int len);
int url_match(const char *url1, const char *url2);
int url_begin_with(const char *url1, const char *url2);

#define range(a, b) for (int i = (a); i < (b); i++)


#endif // REACT_H


/**************************************************/


#ifdef REACT_IMPLEMENTATION
#undef REACT_IMPLEMENTATION


#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>

#ifndef HTML_PAGE_MAX_SIZE
#define HTML_PAGE_MAX_SIZE 6969
#endif

#define HEADER_LEN 17

typedef struct Html_buffer {
    int len;
    char header[HEADER_LEN];
    char buff[HTML_PAGE_MAX_SIZE];
} Html_buffer;

Html_buffer html_buffer = {0};

#define buffer_add(...) html_buffer.len += snprintf(html_buffer.buff + html_buffer.len, HTML_PAGE_MAX_SIZE - html_buffer.len, __VA_ARGS__)
#define va_buffer_add(format, va) html_buffer.len += vsnprintf(html_buffer.buff + html_buffer.len, HTML_PAGE_MAX_SIZE - html_buffer.len, format, va)

int tab = 0;

void calm_tabbing()
{
    for (int i = 0; i < tab; i++) {
        buffer_add("    ");
    }
}

void tabbing()
{
    calm_tabbing();
    tab++;
}

void untabbing()
{
    tab--;
    calm_tabbing();
}

#define gen_simple_balise_func(name)            \
    int name##_begin(const char *attr)          \
    {                                           \
        tabbing();                              \
        if (*attr == '\0')                      \
            buffer_add("<"#name">\n");          \
        else                                    \
            buffer_add("<"#name" %s>\n", attr); \
        return 1;                               \
    }                                           \
    void name##_end()                           \
    {                                           \
        untabbing();                            \
        buffer_add("</"#name">\n");             \
    }

#define X(balise) gen_simple_balise_func(balise);
BALISE

#undef X
#undef BALISE
#undef gen_simple_balise_func

void doctype()
{
    calm_tabbing();
    buffer_add("<!DOCTYPE html>\n");
}

void text(const char *txt)
{
    calm_tabbing();
    buffer_add("%s\n", txt);
}

void textf(const char *format, ...)
{
    calm_tabbing();
    va_list vaarg;
    va_start(vaarg, format);
    va_buffer_add(format, vaarg);
    va_end(vaarg);
}

void multi_line_text_imp(const char *first_txt, ...)
{
    calm_tabbing();
    buffer_add("%s\n", first_txt);

    va_list vaarg;
    va_start(vaarg, first_txt);
    const char *arg = va_arg(vaarg, const char *);

    while (arg) {
        calm_tabbing();
        buffer_add("%s\n", arg);
        arg = va_arg(vaarg, const char *);
    }

    va_end(vaarg);
}

int html_base_template_imp(const char *title)
{
    doctype();
    html_begin("lang='en'");
    {
        head("") {
            meta("charset='UTF-8'");
            meta("name='viewport' content='width=device-width, initial-scale=1.0'");
            meta("http-equiv='X-UA-Compatible' content='ie=edge'");
            title("") {
                text(title);
            }
            html_link("rel='stylesheet' href='/style.css'");
            html_link("rel='icon' href='/favicon.ico' type='image/x-icon'");
            script("") {
                multi_line_text(
                    "function truc() {",
                    "   document.querySelectorAll('[get]').forEach(e => {",
                    "       e.onclick = async function() {",
                    "           var req = await fetch(e.getAttribute('get'));",
                    "           var txt = await req.text();",
                    "           e.outerHTML = txt;",
                    "           truc();",
                    "       };",
                    "   });",
                    "};"
                );
                multi_line_text(
                    "window.onload = function () {",
                    "   truc();",
                    "};"
                );
            }
        }
        body_begin("");
    }

    return 1;
}


int server_fd, new_socket;
struct sockaddr_in address;
int addrlen = sizeof(address);

int open_server(int port)
{
    int opt = 1;

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return -1;
    }

    // Attach socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return -1;
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        return -1;
    }

    fprintf(stdout, "Server listening on port %d\n", port);
    return 0;
}

int accept_connection()
{
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        return -1;
    }
    return 0;
}

void close_connection()
{
    close(new_socket);
}

void close_server()
{
    close(server_fd);
}

int read_request()
{
    html_buffer.len = 0;
    return read(new_socket, html_buffer.buff, HTML_PAGE_MAX_SIZE);
}

int write_request()
{
    return write(new_socket, html_buffer.header, html_buffer.len + HEADER_LEN);
}

void http_request_code(int code)
{
    snprintf(html_buffer.header, HEADER_LEN + 1, "HTTP/1.0 %.3d OK\n\n", code);
}

void print_html()
{
    printf("%.*s", html_buffer.len, html_buffer.buff);
}

int save_html(const char *file_name)
{
    FILE *file = fopen(file_name, "w");
    if (file == NULL)
        return -1;

    fprintf(file, "%.*s", html_buffer.len, html_buffer.buff);

    fclose(file);
    return 0;
}

int get_path(char *buf, int len)
{
    if (html_buffer.len != 0)
        return -1;
    
    int offset = 4;
    int count = 0;
    while (count < len && html_buffer.buff[count + offset] != ' ')
        count++;

    return snprintf(buf, len, "%.*s", count, html_buffer.buff + offset);
}

int url_match(const char *url1, const char *url2)
{
    int len1 = strlen(url1);
    int len2 = strlen(url2);
    for (int i = 0; i < len1 || i < len2; i++) {
        if (url1[i] != url2[i])
            return 0;
    }
    return 1;
}

int url_begin_with(const char *url1, const char *url2)
{
    int len1 = strlen(url1);
    int len2 = strlen(url2);
    if (len2 < len1)
        return 0;
    for (int i = 0; i < len1; i++) {
        if (url1[i] != url2[i])
            return 0;
    }
    return 1;
}


#endif // REACT_IMPLEMENTATION
