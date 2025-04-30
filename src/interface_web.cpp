#include<lib.h>

String htmlPage = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="UTF-8">
      <title>Joystick ESP32</title>
      <style>
        #joystickZone {
          width: 200px;
          height: 200px;
          background: #eee;
          border-radius: 50%;
          position: relative;
          margin: 50px auto;
          touch-action: none;
        }
    
        #stick {
          width: 60px;
          height: 60px;
          background: #333;
          border-radius: 50%;
          position: absolute;
          top: 70px;
          left: 70px;
        }
    
        #values {
          text-align: center;
          font-family: sans-serif;
          font-size: 1.2em;
        }
      </style>
    </head>
    <body>
      <h1 style="text-align:center;">Joystick ESP32</h1>
      <div id="joystickZone">
        <div id="stick"></div>
      </div>
      <div id="values">
        X: <span id="xVal">0</span> |
        Y: <span id="yVal">0</span>
      </div>
    
      <script>
        const zone = document.getElementById("joystickZone");
        const stick = document.getElementById("stick");
        const xVal = document.getElementById("xVal");
        const yVal = document.getElementById("yVal");
    
        let centerX = zone.offsetWidth / 2;
        let centerY = zone.offsetHeight / 2;
    
        let maxRange = 70; // rayon max pour le joystick
    
        zone.addEventListener("touchmove", function(event) {
          event.preventDefault();
          let touch = event.touches[0];
          let rect = zone.getBoundingClientRect();
          let x = touch.clientX - rect.left - centerX;
          let y = touch.clientY - rect.top - centerY;
    
          // Limiter à la zone circulaire
          const dist = Math.sqrt(x * x + y * y);
          if (dist > maxRange) {
            x *= maxRange / dist;
            y *= maxRange / dist;
          }
    
          stick.style.left = (x + centerX - 30) + "px";
          stick.style.top = (y + centerY - 30) + "px";
    
          // Affichage
          xVal.innerText = Math.round(x);
          yVal.innerText = Math.round(y);
    
          // Optionnel : envoyer les valeurs au serveur
          fetch(`/joy?x=${Math.round(x)}&y=${Math.round(y)}`).catch(e => {});
        });
    
        // Remet le stick au centre quand on relâche
        zone.addEventListener("touchend", () => {
          stick.style.left = "70px";
          stick.style.top = "70px";
          xVal.innerText = 0;
          yVal.innerText = 0;
    
          // Envoyer retour au neutre
          fetch(`/joy?x=0&y=0`).catch(e => {});
        });
      </script>
    </body>
    </html>
    )rawliteral";
    