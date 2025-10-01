/*
 * serial_send.c  – version 1.0
 *
 * Fonctionnalités :
 *   • options -d <device>, -b <baud>, -l (liste des bauds), -h (aide)
 *   • vérification du baud après configuration
 *   • boucle full‑duplex (select() sur stdin + le port série)
 *
 * Compilation :
 *     gcc -Wall -O2 -o serial_chat_full serial_chat_full.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/select.h>
#include <getopt.h>

#define DEFAULT_DEVICE   "/dev/ttyUSB0"
#define DEFAULT_BAUD     38400          /* valeur numérique */
#define MAX_LINE         1024

/* -------------------------------------------------------------------------- */
/* Table des bauds supportés – utilisée par -l et par la conversion */
static const struct {
    int      baud;
    speed_t  constant;
} baud_table[] = {
    { 1200,   B1200   },
    { 2400,   B2400   },
    { 4800,   B4800   },
    { 9600,   B9600   },
    { 19200,  B19200  },
    { 38400,  B38400  },
    { 57600,  B57600  },
    { 115200, B115200 },
    { 230400, B230400 },
    { 460800, B460800 },
    { 921600, B921600 }
};
#define BAUD_TABLE_SIZE (sizeof(baud_table)/sizeof(baud_table[0]))

/* -------------------------------------------------------------------------- */
static speed_t baud_to_constant(int baud)
{
    for (size_t i = 0; i < BAUD_TABLE_SIZE; ++i)
        if (baud_table[i].baud == baud)
            return baud_table[i].constant;

    fprintf(stderr,
            "⚠️  Baud %d non supporté – utilisation du défaut %d.\n",
            baud, DEFAULT_BAUD);
    return B38400;   /* fallback */
}

/* -------------------------------------------------------------------------- */
static void print_supported_bauds(void)
{
    puts("Bauds supportés (valeur décimale → constante termios) :");
    for (size_t i = 0; i < BAUD_TABLE_SIZE; ++i)
        printf("  %6d → %s\n", baud_table[i].baud,
               (baud_table[i].constant == B38400)   ? "B38400"   :
               (baud_table[i].constant == B115200) ? "B115200"  :
               (baud_table[i].constant == B230400) ? "B230400"  :
               (baud_table[i].constant == B460800) ? "B460800"  :
               (baud_table[i].constant == B921600) ? "B921600"  :
               "autre");
}

/* -------------------------------------------------------------------------- */
static int init_serial(const char *device, int baudrate)
{
    int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }

    /* --- Vitesse -------------------------------------------------------- */
    speed_t speed = baud_to_constant(baudrate);
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    /* --- Format 8N1, pas de contrôle de flux --------------------------- */
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;   /* 8 bits de données */
    tty.c_cflag &= ~PARENB;                       /* pas de parité */
    tty.c_cflag &= ~CSTOPB;                       /* 1 bit de stop */
    tty.c_cflag &= ~CRTSCTS;                      /* pas de RTS/CTS */
    tty.c_cflag |= CREAD | CLOCAL;                /* activer la lecture, ignorer le contrôle de ligne */

    /* --- Mode RAW (pas de transformation) ----------------------------- */
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL | INLCR | IGNCR);
    tty.c_oflag &= ~OPOST;

    /* --- Timeout de lecture (0,5 s) ------------------------------------ */
    tty.c_cc[VMIN]  = 0;   /* aucun octet minimum requis */
    tty.c_cc[VTIME] = 5;   /* décisecondes → 0,5 s */

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }

    /* Vérification post‑configuration (facultative mais utile) */
    speed_t cur_in  = cfgetispeed(&tty);
    speed_t cur_out = cfgetospeed(&tty);
    if (cur_in != speed || cur_out != speed) {
        fprintf(stderr,
                "⚠️  Le baud demandé (%d) n’a pas pu être appliqué exactement.\n",
                baudrate);
    }

    tcflush(fd, TCIOFLUSH);   /* vider les tampons */
    return fd;
}

/* -------------------------------------------------------------------------- */
static void print_usage(const char *progname)
{
    printf(
        "Usage: %s [options]\n"
        "\nOptions :\n"
        "  -d <device>   Chemin du périphérique série (défaut : %s)\n"
        "  -b <baud>     Baudrate (défaut : %d). Voir -l pour la liste.\n"
        "  -l            Lister les baudrates supportés et quitter.\n"
        "  -h            Afficher cette aide.\n"
        "\nExemples :\n"
        "  %s                     # /dev/ttyUSB0 @ 38400\n"
        "  %s -d /dev/ttyUSB1    # même baud, autre device\n"
        "  %s -b 115200          # 115200 baud\n"
        "  %s -l                 # afficher les bauds supportés\n",
        progname, DEFAULT_DEVICE, DEFAULT_BAUD,
        progname, progname, progname, progname);
}

/* -------------------------------------------------------------------------- */
int main(int argc, char *argv[])
{
    const char *device = DEFAULT_DEVICE;
    int baud = DEFAULT_BAUD;

    /* ---------- Traitement des options ---------- */
    int opt;
    while ((opt = getopt(argc, argv, "d:b:lh")) != -1) {
        switch (opt) {
            case 'd':
                device = optarg;
                break;
            case 'b': {
                char *endptr = NULL;
                long v = strtol(optarg, &endptr, 10);
                if (*endptr != '\0' || v <= 0) {
                    fprintf(stderr,
                            "⚠️  Baud invalide \"%s\" – utilisation du défaut %d.\n",
                            optarg, DEFAULT_BAUD);
                } else {
                    baud = (int)v;
                }
                break;
            }
            case 'l':
                print_supported_bauds();
                return EXIT_SUCCESS;
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    /* ---------- Ouverture et configuration du port ---------- */
    int fd = init_serial(device, baud);
    if (fd < 0) {
        fprintf(stderr, "❌  Impossible d’ouvrir le port %s\n", device);
        return EXIT_FAILURE;
    }

    printf("✅  Port %s ouvert à %d baud.\n", device, baud);
    printf("Tapez du texte, appuyez sur <Entrée> → envoi.\n");
    printf("Les réponses du périphérique seront affichées immédiatement.\n");
    printf("Ctrl‑D (EOF) pour quitter.\n\n");

    /* ---------- Boucle full‑duplex (stdin ↔ port série) ---------- */
    char line[MAX_LINE];
    while (1) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(fd, &read_fds);

        int maxfd = (STDIN_FILENO > fd) ? STDIN_FILENO : fd;
        int sel = select(maxfd + 1, &read_fds, NULL, NULL, NULL);
        if (sel < 0) {
            perror("select");
            break;
        }

        /* ----- Données provenant du clavier (stdin) ----- */
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(line, sizeof(line), stdin) == NULL) {   /* EOF (Ctrl‑D) */
                printf("\n🔚  Fin de l’entrée utilisateur – fermeture du port.\n");
                break;
            }

            size_t len = strlen(line);
            if (len == 0) continue;   /* rien à envoyer */

            ssize_t w = write(fd, line, len);
            if (w < 0) {
                perror("write");
                break;
            }
            printf("[→] %zd octet(s) envoyé(s).\n", w);
        }

        /* ----- Données provenant du port série ----- */
        if (FD_ISSET(fd, &read_fds)) {
            char resp[MAX_LINE];
            ssize_t r = read(fd, resp, sizeof(resp) - 1);
            if (r < 0) {
                perror("read");
                break;
            } else if (r == 0) {
                printf("[←] (lecture retournée 0 octet – périphérique fermé?)\n");
                continue;
            }

            resp[r] = '\0';
            while (r > 0 && (resp[r-1] == '\n' || resp[r-1] == '\r')) {
                resp[--r] = '\0';
            }

            printf("[←] %zd octet(s) reçu(s) : %s\n", r, resp);
        }
    }

    close(fd);
    printf("\n🔚  Port fermé. Au revoir.\n");
    return EXIT_SUCCESS;
}