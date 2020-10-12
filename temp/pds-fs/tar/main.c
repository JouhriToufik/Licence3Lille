#include<tah.h>
#include<arrondi.h>

/*
    fd == 0 == STDIN_FILENO -> STANDARD INPUT
    fd == 1 -> STANDARD OUTPUT
    fd == 2 -> ERROR	
*/

int main(int argc, char *argv[]) {
    int fd;
    struct posix_header hs;

    if (argc > 1 ) {
        fd = open(argv[1], O_RDONLY);
        if (fd < 1) {
            printf("Cannot open file %s\n", argv[1]);
            exit(-1);
        }
    } else {
        fd = STDIN_FILENO; // utiliser un pipe pour mettre des trucs dans
        // l'entrée standard de ./lstar si on passe dans le else
    }


    // boucle infini qui tourne tant que tout les fichiers n'ont pas été analysés
    for (;;) {
       // lit un bloc de 512 octets qui correspond à la description d'un fichier
       read(fd, &hs, sizeof(hs));

       // retrouve la taille d'un fichier (octets) et détermine le nombre de blocs
       // de 512 octets qu'il remplit après le bloc de description
       long long file_size = strtoll(hs.size, NULL, 8);
       int file_size_arrondie = arrondi512(file_size);
       int file_blocks_number = file_size_arrondie / BUFFER;

       // récupération de la version du fichier à utiliser pour vérifier que le fichier
       // en est bien un. Obligé de faire comme ça car la cimple récupération de hs.version
       // donne une String plus longue que ce qu'on veut
       char version[2];
       memcpy( version, &hs.version[0], 2 );

       // vérfie que le bloc lu correspond bien à une description de fichier,
       // si ce n'est pas le cas, on arrete la boucle
       if (strcmp(TMAGIC,hs.magic) != 0 || strcmp(TVERSION,version) != 0) {
          break;
       }

       // affichage des informations du fichier comme tar -tv.

        //recuperation et affichage du mode fichier
        int long mode = strtol(hs.mode,NULL,8);


        printf((S_ISDIR(mode)) ? "d" : "-");
        printf((mode&S_IRUSR) ? "r" : "-");
        printf((mode&S_IWUSR) ? "w" : "-");
        printf((mode&S_IXUSR) ? "x" : "-");
        printf((mode&S_IRGRP) ? "r" : "-");
        printf((mode&S_IWGRP) ? "w" : "-");
        printf((mode&S_IXGRP) ? "x" : "-");
        printf((mode&S_IROTH) ? "r" : "-");
        printf((mode&S_IWOTH) ? "w" : "-");
        printf((mode&S_IXOTH) ? "X" : "-");
        printf("\t");

        //affichage de prefix du fichier
            printf("%s", strcat(strcat(hs.prefix,"/"),hs.uname));
        //affichage Nom du fichier
            printf(" %s \t", hs.name);
        //affichage Taille du fichier
            printf(" %lld  ", file_size);
        //recuperation et affichage du date fichier
        struct tm *info;
        char buffer[DATE_BUFFER_SIZE]; //un buffer pour le stockage du format date à 12 octets
        time_t rawtime= strtol(hs.mtime,NULL, 8);
        info = localtime(&rawtime);
        printf( "%s ", asctime(info)); // ou avec fonction strftime(buffer, DATE_BUFFER_SIZE, "%d/%m/%Y %H:%M:%S", info),mais il bug dans mon programme;

        //recuperation et affichage du type de fichier
        char flagTypeVal= hs.typeflag;

       switch (flagTypeVal)
	{
	    case  '0':
	    case '\0':
		printf("type du fichier: regular file\n");
		break;
	    case '1':
		printf("type du fichier: Link\n");
		break;
	    case '2':
	    case '7':
		printf("type du fichier: reserved\n");
		break;
	    case '3':
		printf("type du fichier: Character special\n");
		break;
	    case '4':
		printf("type du fichier: Block special\n");
		break;
	    case '5':
		printf("type du fichier: Directory\n");
		break;
	    case '6':
		printf("type du fichier: FIFO\n");
		break;

	    default:
		printf("type du fichier: Error! flag type is not correct\n");



	}


    // on continue de lire le fichier autant de fois qu'il
       // possède de blocs de données de 512 octets pour les passer sans les regarder
       int i;
       for(i = 0; i < file_blocks_number ; i++) {
          read(fd, &hs, sizeof(hs));
       }
    }

    close(fd);

    return 0;
}
