#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <string.h>
#include "lodepng.h"
#include "energy.h"
#include "seam_carver.h"
#include "effects.h"

#define ANIMATION 1

enum KeyPressSurfaces
{
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};

unsigned char *rgba_make_copy(unsigned int width, unsigned int height, char *source)
{
    unsigned char *copy = malloc(sizeof(char) * 4 * width * height);
    assert(copy != NULL); 

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            copy[(4 * i ) + (4 * j * width) + 0] = source[(4 * i ) + (4 * j * width) + 0];
            copy[(4 * i ) + (4 * j * width) + 1] = source[(4 * i ) + (4 * j * width) + 1];
            copy[(4 * i ) + (4 * j * width) + 2] = source[(4 * i ) + (4 * j * width) + 2];
            copy[(4 * i ) + (4 * j * width) + 3] = source[(4 * i ) + (4 * j * width) + 3]; 
        }
    }
    return copy;
}

float *rgba_make_energy_map(unsigned int width, unsigned int height, char *source)
{
    float *gradient = malloc(sizeof(float) * width * height);
    assert(gradient != NULL);
    sobel(source, width, height, gradient);
    return gradient;
}

void rgba_write(unsigned int width, unsigned int height, char *source, char *name)
{
    unsigned char *png = 0;
    size_t pngsize;
    unsigned int err = lodepng_encode32(&png, &pngsize, source, width, height);
    if (err != 0) {
        printf("Could not encode %s. LodePNG_Error: %u, %s\n", name, err, lodepng_error_text(err));
        return 1;
    }
    lodepng_save_file(png, pngsize, name);
    free(png);
}

void missing_arg_handler(char *argv0)
{
    printf("%s: missing operand\nTry '%s --help' for more information.\n", argv0, argv0);
}

void help_handler(char *argv0)
{
    printf("usage: %s FILENAME [-rr] [-re] [-rs] [-tw TARGET_WIDTH] [-th TARGET_HEIGHT]\n"
    "\t-rr: dump intermediate images to disk\n"
    "\t-re: dump intermediate energy maps to disk\n"
    "\t-rs: dump intermediate seams to disk\n"
    "\t-tw TARGET_WIDTH: resize image width to TARGET_WIDTH\n"
    "\t-th TARGET_HEIGHT: resize image height to TARGET_HEIGHT\n"
    "\tomit -tw and -th flags to enter interactive mode\n"
    "\nSeam-carve a PNG image.\n"
    "%s implements Rubinstein, Avidan, and Shamir's forward-energy-based content resizing algorithm.\n"
    "Note that FILENAME must be a PNG. The [-tw TARGET_WIDTH] and [-th TARGET_HEIGHT] may be used\n"
    "together or independently (in the latter case %s will maintain the source aspect ratio).\n"
    "\nBy default, %s FILENAME (without target dimensions) launches the editor in interactive mode.\n"
    "\n\tEXIT the editor with Q or ESC.\n"
    "\tSAVE the current VIEW with W.\n"
    "\tToggle SEAMS with S.\n"
    "\tToggle HEATMAP with H.\n", argv0, argv0, argv0);   
}

void syntax_err_handler(char *argv0)
{
    printf("%s: syntax error\nTry '%s --help' for more information.\n", argv0, argv0);
}

char mo[] = "mode:";
char in[] = "interactive";
char au[] = "auto";
char vi[] = "view:";
char hm[] = "heatmap";
char se[] = "seams";
char so[] = "source";
char wi[] = "w:";
char he[] = "h:";
char bu[] = "(busy...)";
char id[] = "(idle)";
char qu[] = "(quitting...)";
char frr[] = "raw";
char fre[] = "energy";
char frs[] = "seam";

char ti[50];
char fna[20];

int main(int argc, char *argv[])
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // parse args
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    int rrflag = 0;
    int reflag = 0;
    int rsflag = 0;
    int twflag = 0;
    int thflag = 0;
    int twargflag = 0;
    int thargflag = 0;
    int iflag = 1;

    int tw = 0;
    int th = 0;

    int fflag = 0;
    char *f;

    if (argc == 1) {
        missing_arg_handler(argv[0]);
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--help")) {
            help_handler(argv[0]);
            return 0;
        } else if (!strcmp(argv[i], "-rr")) {
            if (rrflag) {
                syntax_err_handler(argv[0]);
                return 1;
            } else {
                rrflag = 1;
            }
        } else if (!strcmp(argv[i], "-re")) {
            if (reflag) {
                syntax_err_handler(argv[0]);
                return 1;
            } else {
                reflag = 1;
            }
        } else if (!strcmp(argv[i], "-rs")) {
            if (rsflag) {
                syntax_err_handler(argv[0]);
                return 1;
            } else {
                rsflag = 1;
            }
        } else if (!strcmp(argv[i], "-tw")) {
            if (twflag) {
                syntax_err_handler(argv[0]);
                return 1;
            } else {
                if (i + 1 < argc && (atoi(argv[i + 1]) != 0)) {
                    twflag = 1;
                    tw = atoi(argv[i + 1]);
                    iflag = 0;
                    i++;
                } else {
                    missing_arg_handler(argv[0]);
                    return 1;
                }
            }
        } else if (!strcmp(argv[i], "-th")) {
            if (thflag) {
                syntax_err_handler(argv[0]);
                return 1;
            } else {
                if (i + 1 < argc && (atoi(argv[i + 1]) != 0)) {
                    thflag = 1;
                    th = atoi(argv[i + 1]);
                    iflag = 0;
                    i++;
                } else {
                    missing_arg_handler(argv[0]);
                    return 1;
                }
            }
        } else {
            if (fflag) {
                syntax_err_handler(argv[0]);
                return 1;
            } else {
                fflag = 1;
                f = argv[i];
            }
        }
    }
    
    //printf("Dumping args: rrflag: %d\treflag: %d\trsflag: %d\ttwflag: %d\ttw: %d\tthflag: %d\tth: %d\tiflag: %d\tfflag: %d\tf: %s\n", rrflag, reflag, rsflag, twflag, tw, thflag, th, iflag, fflag, f);

    if (!fflag) {
        missing_arg_handler(argv[0]);
        return 1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // set up SDL context
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    SDL_Surface *workbench = NULL;
    SDL_Surface *heatmap_view = NULL;
    SDL_Surface *seam_view = NULL;
    
    SDL_Window *window = NULL;
    SDL_Surface *window_screen = NULL;
    SDL_Event e;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Could not initialize SDL. SDL_Error: %s\n", SDL_GetError());
        return 1;
    } 

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // decode from disk, get essential dimensional data
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int err, width, height;
    unsigned char *src = 0;
    err = lodepng_decode32_file(&src, &width, &height, f);
    if (err != 0) {
        printf("Could not decode %s. LodePNG_Error: %u, %s\n", f, err, lodepng_error_text(err));
        return 1;
    }

    int init_width = width;
    int init_height = height;

    char *sta = id;
    char *mostr = in;

    if (twflag || thflag) {
        mostr = au;
        sta = bu;
        if (tw > width) {
            printf("Invalid TARGET_WIDTH.\n");
            return 1;
        }
        if (th > height) {
            printf("Invalid TARGET_HEIGHT.\n");
            return 1;
        } 
        if (twflag && !thflag) {
            th = (tw * height) / width;
        }
        if (!twflag && thflag) {
            tw = (th * width) / height;
        }
    }

    float target_ratio = (float)tw / (float)th;
    float actual_ratio = (float)width / (float)height;


    //printf("Dumping args: rrflag: %d\treflag: %d\trsflag: %d\ttwflag: %d\ttw: %d\tthflag: %d\tth: %d\tiflag: %d\tfflag: %d\tf: %s\n", rrflag, reflag, rsflag, twflag, tw, thflag, th, iflag, fflag, f);


    // internal copies
    unsigned char *working_copy = rgba_make_copy(width, height, src);
    unsigned char *heatmap_copy = rgba_make_copy(width, height, src);
    unsigned char *seam_copy = rgba_make_copy(width, height, src);
    unsigned char *original = rgba_make_copy(width, height, src);
    

    // ditch source (protect it)
    free(src);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // back to SDL
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

    window = SDL_CreateWindow(argv[0], SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Could not create window. SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    window_screen = SDL_GetWindowSurface(window);
    SDL_Surface *black = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    SDL_FillRect(black, NULL, SDL_MapRGB(black->format, 0, 0, 0));
    workbench = SDL_CreateRGBSurfaceWithFormatFrom(working_copy, width, height, 32, 4 * width, SDL_PIXELFORMAT_RGBA32);
    SDL_SetSurfaceBlendMode(workbench, SDL_BLENDMODE_BLEND);
    float *energy_map = rgba_make_energy_map(width, height, working_copy);
    float *energy_map_transpose = energy_transpose(width, height, energy_map);
    int *targets_vertical = malloc(sizeof(int) * height);
    seam_carve(height, width, energy_map_transpose, targets_vertical);
    vertical_seam(width, height, seam_copy, targets_vertical);
    heatmap(width, height, heatmap_copy, energy_map);
    heatmap_view = SDL_CreateRGBSurfaceWithFormatFrom(heatmap_copy, width, height, 32, 4 * width, SDL_PIXELFORMAT_RGBA32);
    SDL_SetSurfaceBlendMode(heatmap_view, SDL_BLENDMODE_BLEND);
    int *targets_horizontal = malloc(sizeof(int) * width);
    seam_carve(width, height, energy_map, targets_horizontal);
    horizontal_seam(width, height, seam_copy, targets_horizontal);
    seam_view = SDL_CreateRGBSurfaceWithFormatFrom(seam_copy, width, height, 32, 4 * width, SDL_PIXELFORMAT_RGBA32);
    
    int quit = 0;
    int view = 2;

    char *vistr;
    vistr = so;

    int frame_num = 0;
    
    while(quit == 0) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = !quit;
            } else if ((e.type == SDL_KEYDOWN && iflag)) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE: {
                        quit = !quit;
                        sta = qu;
                        break;
                    }
                    case SDLK_q: {
                        quit = !quit;
                        sta = qu;
                        break;
                    }
                    case SDLK_w: {
                        printf("Writing current view to disk as %s\n", "view.png");
                        if (view == 1) {
                            rgba_write(width, height, heatmap_copy, "view.png");
                        } else if (view == 2) {
                            rgba_write(width, height, seam_copy, "view.png");
                        } else {
                            rgba_write(width, height, working_copy, "view.png");
                        }
                        break;
                    }
                    case SDLK_h: {
                        printf("Toggled heatmap.\n");
                        if (view == 1) {
                            view = 0;
                        } else {
                            view = 1;
                        }
                        break;
                    }
                    case SDLK_s: {
                        printf("Toggled seams.\n");
                        if (view == 2) {
                            view = 0;
                        } else {
                            view = 2;
                        }
                        break;
                    }
                    case SDLK_UP: {
                        // create a new working copy using pre-calculated target buffer
carve_horizontal:       ;unsigned char *new_working_copy = execute_carve_horizontal(width, height, working_copy, targets_horizontal);
                        // free stale copies
                        free(working_copy);
                        free(heatmap_copy);
                        free(seam_copy);
                        // update copies
                        working_copy = new_working_copy;
                        heatmap_copy = rgba_make_copy(width, height, working_copy);
                        seam_copy = rgba_make_copy(width, height, working_copy);
                        // free stale surfaces
                        SDL_FreeSurface(workbench);
                        SDL_FreeSurface(seam_view);
                        // decrement height
                        height--;
                        // update workbench
                        workbench = SDL_CreateRGBSurfaceWithFormatFrom(working_copy, width, height, 32, 4 * width, SDL_PIXELFORMAT_RGBA32);
                        // free stale energy maps
                        free(energy_map);
                        free(energy_map_transpose);
                        // update energy maps
                        energy_map = rgba_make_energy_map(width, height, working_copy);
                        energy_map_transpose = energy_transpose(width, height, energy_map);
                        // update heatmap_copy
                        heatmap(width, height, heatmap_copy, energy_map);
                        // update heatmap_view
                        heatmap_view = SDL_CreateRGBSurfaceWithFormatFrom(heatmap_copy, width, height, 32, 4 * width, SDL_PIXELFORMAT_RGBA32);
                        // update seam_copy
                        free(targets_horizontal);
                        free(targets_vertical);
                        targets_horizontal = malloc(sizeof(int) * width);
                        targets_vertical = malloc(sizeof(int) * height);
                        seam_carve(width, height, energy_map, targets_horizontal);
                        seam_carve(height, width, energy_map_transpose, targets_vertical);
                        horizontal_seam(width, height, seam_copy, targets_horizontal); 
                        vertical_seam(width, height, seam_copy, targets_vertical);
                        // update seam_view
                        seam_view = SDL_CreateRGBSurfaceWithFormatFrom(seam_copy, width, height, 32, 4 * width, SDL_PIXELFORMAT_RGBA32);
                        
                        if (rrflag) {
                            sprintf(fna, "%s_%05d.png", frr, frame_num);
                            lodepng_encode32_file(fna, working_copy, width, height);
                        }
                        if (reflag) {
                            sprintf(fna, "%s_%05d.png", fre, frame_num);
                            lodepng_encode32_file(fna, heatmap_copy, width, height);
                        }
                        if (rsflag) {
                            sprintf(fna, "%s_%05d.png", frs, frame_num);
                            lodepng_encode32_file(fna, seam_copy, width, height);
                        }
                        frame_num++;

                        break;
                    }
                    case SDLK_LEFT: {
                        // create a new working copy using pre-calculated target buffer
carve_vertical:         ;unsigned char *new_working_copy = execute_carve_vertical(width, height, working_copy, targets_vertical);
                        // free stale copies
                        free(working_copy);
                        free(heatmap_copy);
                        free(seam_copy);
                        // update copies
                        working_copy = new_working_copy;
                        heatmap_copy = rgba_make_copy(width, height, working_copy);
                        seam_copy = rgba_make_copy(width, height, working_copy);
                        // free stale surfaces
                        SDL_FreeSurface(workbench);
                        SDL_FreeSurface(seam_view);
                        // decrement width;
                        width--;
                        // update workbench
                        workbench = SDL_CreateRGBSurfaceWithFormatFrom(working_copy, width, height, 32, 4 * width, SDL_PIXELFORMAT_RGBA32);
                        // free stale energy maps
                        free(energy_map);
                        free(energy_map_transpose);
                        // update energy maps
                        energy_map = rgba_make_energy_map(width, height, working_copy);
                        energy_map_transpose = energy_transpose(width, height, energy_map);
                        // update heatmap_copy
                        heatmap(width, height, heatmap_copy, energy_map);
                        // update heatmap_view
                        heatmap_view = SDL_CreateRGBSurfaceWithFormatFrom(heatmap_copy, width, height, 32, 4 * width, SDL_PIXELFORMAT_RGBA32);
                        // update seam_copy
                        free(targets_horizontal);
                        free(targets_vertical);
                        targets_horizontal = malloc(sizeof(int) * width);
                        targets_vertical = malloc(sizeof(int) * height);
                        seam_carve(width, height, energy_map, targets_horizontal);
                        seam_carve(height, width, energy_map_transpose, targets_vertical);
                        horizontal_seam(width, height, seam_copy, targets_horizontal); 
                        vertical_seam(width, height, seam_copy, targets_vertical);
                        // update seam_view
                        seam_view = SDL_CreateRGBSurfaceWithFormatFrom(seam_copy, width, height, 32, 4 * width, SDL_PIXELFORMAT_RGBA32);
                        if (rrflag) {
                            sprintf(fna, "%s_%05d.png", frr, frame_num);
                            lodepng_encode32_file(fna, working_copy, width, height);
                        }
                        if (reflag) {
                            sprintf(fna, "%s_%05d.png", fre, frame_num);
                            lodepng_encode32_file(fna, heatmap_copy, width, height);
                        }
                        if (rsflag) {
                            sprintf(fna, "%s_%05d.png", frs, frame_num);
                            lodepng_encode32_file(fna, seam_copy, width, height);
                        }
                        frame_num++;
                        break;
                    }
                }
            }
        }
        SDL_BlitSurface(black, NULL, window_screen, NULL); 
        if (view == 1) {
            SDL_BlitSurface(heatmap_view, NULL, window_screen, NULL); 
            vistr = hm;
        } else if (view == 2) {
            SDL_BlitSurface(seam_view, NULL, window_screen, NULL);  
            vistr = se;
        } else {
            SDL_BlitSurface(workbench, NULL, window_screen, NULL);
            vistr = so;
        }

        sprintf(ti, "%s %s\t|\t%s %s\t|\t%s %s\t|\t%s %d %s %d", argv[0], sta, mo, mostr, vi, vistr, wi, width, he, height);
        SDL_SetWindowTitle(window, ti);
        SDL_UpdateWindowSurface(window);
        
        // handle auto resize (this is terrible but okay for now)

        if (twflag || thflag) {
            if (width != tw && height != th) {
                float actual_ratio = (float)width / (float)height;
                if (actual_ratio >= target_ratio) {
                    goto carve_vertical;
                } else {
                    goto carve_horizontal;
                }
            }
            if (width != tw) {
                goto carve_vertical;
            } 
            if (height != th) {
                goto carve_horizontal;
            }
            twflag = thflag = 0;
            iflag = 1;
            mostr = in;
            sta = id;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // clean up
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    free(working_copy);
    free(heatmap_copy);
    free(seam_copy);
    SDL_FreeSurface(heatmap_view);
    SDL_FreeSurface(seam_view);
    SDL_FreeSurface(workbench);
    SDL_FreeSurface(black);
    free(targets_vertical);
    free(targets_horizontal);
    free(energy_map);
    free(energy_map_transpose);
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // handle records
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    char rsz[100];
    sprintf(rsz, "mogrify -resize %dx%d -background black -gravity NorthWest -extent %dx%d *.png", init_width, init_height, init_width, init_height);

    char rranim[100];
    sprintf(rranim, "convert -delay 10 -loop 0 raw*.png raw.gif");

    char reanim[100];
    sprintf(reanim, "convert -delay 10 -loop 0 energy*.png energy.gif");

    char rsanim[100];
    sprintf(rsanim, "convert -delay 10 -loop 0 seam*.png seam.gif");


    if ((rrflag || reflag || rsflag) && ANIMATION) {
        printf("resizing...\n");
        system(rsz);
        if (rrflag) {
            printf("animating raw...\n");
            system(rranim);
        }
        if (reflag) {
            printf("animating energy...\n");
            system(reanim);
        }
        if (rsflag) {
            printf("animating seams...\n");
            system(rsanim);
        }
    }
    
    return 0;
}
