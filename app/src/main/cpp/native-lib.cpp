#include <raylib.h>
#include <math.h>
#include <stdio.h>

#ifdef PLATFORM_ANDROID
    #include <android/input.h>
#endif

struct LibroLanzado {
    float x;
    float y;
    float velocidadX;
    float velocidadY;
    bool activo; 
    int texturaIndex; 
};

int main()
{
  InitWindow(800, 450, "buscando la LUZ");
  SetTargetFPS(60);
  InitAudioDevice();
  
  // PARA ANDROID: Deshabilitar tecla ESC y maximizar pantalla táctil
  #ifdef PLATFORM_ANDROID
      SetExitKey(0);
      SetConfigFlags(FLAG_FULLSCREEN_MODE);
  #endif
  
  int playX = 150;
  int playY = 200;
  int gravedad = 1;
  int fuerzaSalto = -15;
  int velocidadY= 0;
  int tamano = 40;
  int velocidad = 10;
  
  Texture2D logoBoss = LoadTexture("LOGOUNI.png");
  float bossX = 600.0f;
  float bossY = 100.0f;
  float bossAncho = 150.0f;
  float bossAlto = 150.0f;
  float bossVelX = -4.0f;
  float bossVelY = 4.0f;

  Texture2D imagenGraciosa = LoadTexture("foto_12.png");
  float memeAncho = 150.0f;
  float memeAlto = 150.0f;

  Texture2D fondo = LoadTexture("uniVIDEOJUEGO.png");
  Texture2D fondoMenu = LoadTexture("fondo_menu.png");
  int fondoX = 0;

  Music musicaRelajante = LoadMusicStream("sans(MP3_160K).mp3.mp3");
  Music cancionRoncar = LoadMusicStream("roncar.mp3");
  Music musicaPerder = LoadMusicStream("Sonido De Pedo (Marianosmeiden)(MP3_160K).mp3.mp3");
  
  Sound sonidoCien = LoadSound("quick-win.mp3.wav");
  Sound sonidoMil = LoadSound("Cash Register.mp3.mp3");
  Sound sonidoGolpe = LoadSound("Bruh sound effect(MP3_160K).mp3.mp3");

  PlayMusicStream(musicaRelajante);
  PlayMusicStream(cancionRoncar);
  SetMusicVolume(musicaRelajante, 0.2f);
  SetMusicVolume(cancionRoncar, 0.8f);
  SetMusicVolume(musicaPerder, 1.0f);

  Texture2D texturasLibros[11];
  texturasLibros[0] = LoadTexture("foto_1.png");
  texturasLibros[1] = LoadTexture("foto_2.png");
  texturasLibros[2] = LoadTexture("foto_3.png");
  texturasLibros[3] = LoadTexture("foto_4.png");
  texturasLibros[4] = LoadTexture("foto_5.png");
  texturasLibros[5] = LoadTexture("foto_6.png");
  texturasLibros[6] = LoadTexture("foto_7.png");
  texturasLibros[7] = LoadTexture("foto_8.png");
  texturasLibros[8] = LoadTexture("foto_9.png");
  texturasLibros[9] = LoadTexture("foto_10.png");
  texturasLibros[10] = LoadTexture("foto_11.png");

  LibroLanzado libros[5];
  for (int i = 0; i < 5; i++) {
      libros[i].activo = false;
  }

  int estadoJuego = 0;
  int dificultad = 1;
  int limiteLanzamiento = 90;
  
  int vida = 100;
  int framesDano = 0;
  int danoMostrado = 0;

  int puntaje = 0;
  int record = 0;
  int frameCounter = 0;
  int timerLanzamiento = 0;
  
  int proximoHitoCien = 100;
  bool sonadoMil = false;

  // Botones táctiles para Android
  Rectangle btnSalto = { 40, 330, 90, 90 };
  Rectangle btnIzquierda = { 550, 330, 90, 90 };
  Rectangle btnDerecha = { 670, 330, 90, 90 };

  FILE *archivoLectura = fopen("puntuacion.txt", "r");
  if (archivoLectura != NULL) {
      fscanf(archivoLectura, "%d", &record);
      fclose(archivoLectura);
  }

  while (!WindowShouldClose()) {
    
    if (estadoJuego == 2) {
        UpdateMusicStream(musicaPerder);
    } else {
        UpdateMusicStream(musicaRelajante);
        UpdateMusicStream(cancionRoncar);
    }

    if (estadoJuego == 0) {
        Vector2 raton = GetMousePosition();
        
        // En Android, usar toques táctiles para el menú
        #ifdef PLATFORM_ANDROID
            bool click = (GetTouchPointCount() > 0);
            Vector2 puntoToque = GetTouchPosition(0);
            raton = puntoToque;
        #else
            bool click = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        #endif
        
        Rectangle btnFacil = { 300, 180, 200, 50 };
        Rectangle btnMedio = { 300, 250, 200, 50 };
        Rectangle btnDificil = { 300, 320, 200, 50 };

        if (click) {
            bool arrancar = false;
            if (CheckCollisionPointRec(raton, btnFacil)) { dificultad = 1; limiteLanzamiento = 90; arrancar = true; }
            if (CheckCollisionPointRec(raton, btnMedio)) { dificultad = 2; limiteLanzamiento = 60; arrancar = true; }
            if (CheckCollisionPointRec(raton, btnDificil)) { dificultad = 3; limiteLanzamiento = 30; arrancar = true; }
            
            if (arrancar) {
                estadoJuego = 1;
                playX = 150;
                playY = 200;
                velocidadY = 0;
                bossX = 600.0f;
                bossY = 100.0f;
                puntaje = 0;
                vida = 100;
                frameCounter = 0;
                timerLanzamiento = 0;
                proximoHitoCien = 100;
                sonadoMil = false;
                for (int i = 0; i < 5; i++) libros[i].activo = false;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(fondoMenu, 0, 0, WHITE);
        
        DrawRectangleRec(btnFacil, CheckCollisionPointRec(raton, btnFacil) ? LIGHTGRAY : GRAY);
        DrawRectangleRec(btnMedio, CheckCollisionPointRec(raton, btnMedio) ? LIGHTGRAY : GRAY);
        DrawRectangleRec(btnDificil, CheckCollisionPointRec(raton, btnDificil) ? LIGHTGRAY : GRAY);
        
        DrawText("FÁCIL", 365, 195, 20, BLACK);
        DrawText("MEDIO", 365, 265, 20, BLACK);
        DrawText("DIFÍCIL", 360, 335, 20, BLACK);
        
        DrawText(TextFormat("RÉCORD: %i", record), 10, 10, 20, YELLOW);
        EndDrawing();

    } else if (estadoJuego == 1) {
        fondoX -= 2;
        if (fondoX <= -800) {
          fondoX = 0;
        }
        
        bossX += bossVelX;
        bossY += bossVelY;
        
        if (bossX < 400.0f || bossX > (800.0f - bossAncho)) bossVelX *= -1;
        if (bossY < 0.0f || bossY > (450.0f - bossAlto)) bossVelY *= -1;

        // CONTROLES TÁCTILES PARA ANDROID
        #ifdef PLATFORM_ANDROID
            int toques = GetTouchPointCount();
            bool toqueIzquierda = false;
            bool toqueDerecha = false;
            bool toqueSalto = false;
            
            for (int i = 0; i < toques; i++) {
                Vector2 punto = GetTouchPosition(i);
                if (CheckCollisionPointRec(punto, btnIzquierda)) toqueIzquierda = true;
                if (CheckCollisionPointRec(punto, btnDerecha)) toqueDerecha = true;
                if (CheckCollisionPointRec(punto, btnSalto)) {
                    // Solo detectar presión, no mantener
                    static int frameAnterior = 0;
                    if (frameAnterior != GetFrameTime()) {
                        toqueSalto = true;
                        frameAnterior = GetFrameTime();
                    }
                }
            }
            
            // En Android, usar teclas virtuales y también teclas físicas si existen (teclado Bluetooth)
            bool teclaDerecha = IsKeyDown(KEY_RIGHT);
            bool teclaIzquierda = IsKeyDown(KEY_LEFT);
            bool teclaSalto = IsKeyPressed(KEY_SPACE);
            
            if ((teclaDerecha || toqueDerecha) && playX < (800 - tamano)) playX += velocidad;
            if ((teclaIzquierda || toqueIzquierda) && playX > 0) playX -= velocidad;
            if (teclaSalto || toqueSalto) velocidadY = fuerzaSalto;
        #else
            // En PC, usar mouse para los botones táctiles (simulación)
            Vector2 toquePos = GetMousePosition();
            bool toqueIzquierda = IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(toquePos, btnIzquierda);
            bool toqueDerecha = IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(toquePos, btnDerecha);
            bool toqueSalto = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(toquePos, btnSalto);

            if ((IsKeyDown(KEY_RIGHT) || toqueDerecha) && playX < (800 - tamano)) playX += velocidad;
            if ((IsKeyDown(KEY_LEFT) || toqueIzquierda) && playX > 0) playX -= velocidad;
            if (IsKeyPressed(KEY_SPACE) || toqueSalto) velocidadY = fuerzaSalto;
        #endif

        velocidadY += gravedad;
        playY += velocidadY;

        if (playY >= 450 - tamano) {
            playY = 450 - tamano;
            velocidadY = 0;
        }

        if (playY <= 0) {
            playY = 0;
            velocidadY = 0;
        }

        frameCounter++;
        if (frameCounter >= 60) {
            puntaje += 10;
            frameCounter = 0;
            
            if (puntaje >= proximoHitoCien) {
                PlaySound(sonidoCien);
                proximoHitoCien += 100;
            }
            if (puntaje >= 1000 && !sonadoMil) {
                PlaySound(sonidoMil);
                sonadoMil = true;
            }
        }

        timerLanzamiento++;
        if (timerLanzamiento >= limiteLanzamiento) {
            for (int i = 0; i < 5; i++) {
                if (!libros[i].activo) {
                    libros[i].activo = true;
                    libros[i].x = bossX;
                    libros[i].y = bossY + (bossAlto / 2.0f);
                    libros[i].velocidadX = (float)GetRandomValue(-25, -15);
                    libros[i].velocidadY = (float)GetRandomValue(-15, -2);
                    libros[i].texturaIndex = GetRandomValue(0, 10);
                    timerLanzamiento = 0;
                    break;
                }
            }
        }

        for (int i = 0; i < 5; i++) {
            if (libros[i].activo) {
                libros[i].velocidadY += gravedad;
                libros[i].x += libros[i].velocidadX;
                libros[i].y += libros[i].velocidadY;

                if (libros[i].x < -50 || libros[i].y > 450) {
                    libros[i].activo = false;
                }
            }
        }

        Rectangle cajaJugador = { (float)playX, (float)playY, (float)tamano, (float)tamano };
        Rectangle cajaBoss = { bossX + 20, bossY + 20, bossAncho - 40, bossAlto - 40 };
        
        bool impactoActual = false;

        if (CheckCollisionRecs(cajaJugador, cajaBoss)) impactoActual = true;

        for (int i = 0; i < 5; i++) {
            if (libros[i].activo) {
                Rectangle cajaLibro = { libros[i].x, libros[i].y, 50.0f, 50.0f };
                if (CheckCollisionRecs(cajaJugador, cajaLibro)) {
                    impactoActual = true;
                    libros[i].activo = false; 
                }
            }
        }
        
        if (impactoActual && framesDano == 0) {
            PlaySound(sonidoGolpe);
            danoMostrado = GetRandomValue(15, 30);
            vida -= danoMostrado;
            framesDano = 45;
            
            if (vida <= 0) {
                vida = 0;
                estadoJuego = 2;
                PauseMusicStream(musicaRelajante);
                PauseMusicStream(cancionRoncar);
                PlayMusicStream(musicaPerder);
                
                if (puntaje > record) {
                    record = puntaje;
                    FILE *archivoEscritura = fopen("puntuacion.txt", "w");
                    if (archivoEscritura != NULL) {
                        fprintf(archivoEscritura, "%d", record);
                        fclose(archivoEscritura);
                    }
                }
            }
        }

        if (framesDano > 0) framesDano--;

        BeginDrawing(); 
        ClearBackground(RAYWHITE);
        DrawTexture(fondo, fondoX, 0, WHITE);
        DrawTexture(fondo, fondoX + 800, 0, WHITE);
        
        Rectangle origenBoss = { 0.0f, 0.0f, (float)logoBoss.width, (float)logoBoss.height };
        Rectangle destinoBoss = { bossX, bossY, bossAncho, bossAlto };
        Vector2 centroBoss = { 0.0f, 0.0f };
        DrawTexturePro(logoBoss, origenBoss, destinoBoss, centroBoss, 0.0f, WHITE);
        
        for (int i = 0; i < 5; i++) {
            if (libros[i].activo) {
                int indice = libros[i].texturaIndex;
                Rectangle origenLibro = { 0.0f, 0.0f, (float)texturasLibros[indice].width, (float)texturasLibros[indice].height };
                Rectangle destinoLibro = { libros[i].x, libros[i].y, 50.0f, 50.0f };
                Vector2 centroLibro = { 0.0f, 0.0f };
                DrawTexturePro(texturasLibros[indice], origenLibro, destinoLibro, centroLibro, 0.0f, WHITE);
            }
        }

        DrawRectangle(playX, playY, tamano, tamano, RED);
        
        if (dificultad == 2) DrawRectangle(0, 0, 800, 450, Fade(LIGHTGRAY, 0.3f));
        if (dificultad == 3) DrawRectangle(0, 0, 800, 450, Fade(GRAY, 0.6f));

        DrawRectangleRec(btnSalto, Fade(WHITE, 0.3f));
        DrawRectangleLinesEx(btnSalto, 2, Fade(BLACK, 0.4f));
        DrawText("^", btnSalto.x + 35, btnSalto.y + 25, 40, Fade(BLACK, 0.5f));

        DrawRectangleRec(btnIzquierda, Fade(WHITE, 0.3f));
        DrawRectangleLinesEx(btnIzquierda, 2, Fade(BLACK, 0.4f));
        DrawText("<", btnIzquierda.x + 35, btnIzquierda.y + 30, 30, Fade(BLACK, 0.5f));

        DrawRectangleRec(btnDerecha, Fade(WHITE, 0.3f));
        DrawRectangleLinesEx(btnDerecha, 2, Fade(BLACK, 0.4f));
        DrawText(">", btnDerecha.x + 35, btnDerecha.y + 30, 30, Fade(BLACK, 0.5f));

        DrawFPS(10, 10);
        DrawText(TextFormat("PUNTAJE: %i", puntaje), 10, 40, 20, BLACK);
        DrawText(TextFormat("VIDA: %i", vida), 10, 70, 20, (vida > 30) ? BLUE : RED);
        
        if (framesDano > 0) {
            DrawText(TextFormat("-%i", danoMostrado), playX, playY - 30, 25, RED);
        }

        EndDrawing();

    } else if (estadoJuego == 2) {
        // Para Android: tocar cualquier parte inferior para reiniciar
        #ifdef PLATFORM_ANDROID
            bool continuar = false;
            int toques = GetTouchPointCount();
            for (int i = 0; i < toques; i++) {
                Vector2 punto = GetTouchPosition(i);
                if (punto.y > 350) continuar = true;
            }
            if (continuar || IsKeyPressed(KEY_ENTER)) {
                estadoJuego = 0;
                StopMusicStream(musicaPerder);
                ResumeMusicStream(musicaRelajante);
                ResumeMusicStream(cancionRoncar);
            }
        #else
            if (IsKeyPressed(KEY_ENTER) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMousePosition().y > 350)) {
                estadoJuego = 0;
                StopMusicStream(musicaPerder);
                ResumeMusicStream(musicaRelajante);
                ResumeMusicStream(cancionRoncar);
            }
        #endif

        BeginDrawing();
        DrawRectangle(0, 0, 800, 450, Fade(BLACK, 0.8f));
        DrawText("HAS SIDO REPROBADO", 200, 100, 30, RED);
        DrawText("Presiona ENTER (o toca abajo) para volver", 140, 150, 25, WHITE);
        
        Rectangle origenMeme = { 0.0f, 0.0f, (float)imagenGraciosa.width, (float)imagenGraciosa.height };
        Rectangle destinoMeme = { 400.0f - (memeAncho/2), 200.0f, memeAncho, memeAlto };
        Vector2 centroMeme = { 0.0f, 0.0f };
        DrawTexturePro(imagenGraciosa, origenMeme, destinoMeme, centroMeme, 0.0f, WHITE);
        
        DrawText(TextFormat("Puntaje Final: %i", puntaje), 300, 370, 25, GREEN);
        EndDrawing();
    }
  }
  
  UnloadMusicStream(musicaRelajante);
  UnloadMusicStream(cancionRoncar);
  UnloadMusicStream(musicaPerder);
  UnloadSound(sonidoCien);
  UnloadSound(sonidoMil);
  UnloadSound(sonidoGolpe);
  CloseAudioDevice();
  UnloadTexture(fondo);
  UnloadTexture(fondoMenu);
  UnloadTexture(logoBoss);
  UnloadTexture(imagenGraciosa);
  for (int i = 0; i < 11; i++) {
      UnloadTexture(texturasLibros[i]);
  }
  CloseWindow();
  return 0;
}
