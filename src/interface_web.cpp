#include<lib.h>
//Dossier CPP qui stoque le fichier HTML en string pour le server web 

String htmlPage = R"rawliteral(
    
<!-- ecrire le HTML ICI : -->
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8" />
  <title>Interface Utilisateur</title>
  <style>
    body {
      font-family: sans-serif;
      background-color: #f4f4f4;
      margin: 0;
      padding: 0;
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    h1 {
      color: #333;
      margin-top: 30px;
    }

    #joystickZone {
      width: 300px;
      height: 300px;
      background: #ddd;
      border-radius: 50%;
      position: relative;
      margin: 20px;
      border: 2px solid #ccc;
    }

    #stick {
      width: 80px;
      height: 80px;
      background: #333;
      border-radius: 50%;
      position: absolute;
      top: 110px;
      left: 110px;
      cursor: pointer;
    }

    #values {
      font-size: 1.2em;
      margin-top: 20px;
    }

    #values span {
      font-weight: bold;
    }

    .button-container {
      display: flex;
      justify-content: center;
      gap: 10px;
      flex-wrap: wrap;
      margin: 20px 0;
    }

    /* Boutons principaux (bleu) */
    .button-container button,
    /* boutons fermer/envoyer dans modales (mis en commun) */
    .modal-footer button {
      padding: 15px 30px;
      font-size: 1.1em;
      background-color: #007bff;
      color: white;
      border: none;
      border-radius: 8px;
      cursor: pointer;
      transition: background-color 0.3s, transform 0.2s;
    }

    .button-container button:hover,
    .modal-footer button:hover {
      background-color: #0069d9;
      transform: scale(1.05);
    }

    .button-container button:active,
    .modal-footer button:active {
      background-color: #005cbf;
      transform: scale(1);
    }

    .modal {
      display: none;
      position: fixed;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
      background: white;
      padding: 20px;
      border-radius: 8px;
      box-shadow: 0 4px 6px rgba(0, 0, 0, 0.3);
      width: 300px;
      z-index: 1000;
    }

    .modal-header {
      font-size: 1.4em;
      color: #007bff;
      margin-bottom: 10px;
      border-bottom: 2px solid #ccc;
      padding-bottom: 5px;
    }

    .modal-body input {
      width: 100%;
      padding: 10px;
      margin: 8px 0;
      font-size: 1em;
      border-radius: 5px;
      border: 1px solid #ccc;
      box-sizing: border-box;
    }

    /* Boutons secondaires (verts) dans modales - séquences */
    .modal-body button:not(.blue-button) {
      width: 100%;
      padding: 12px;
      font-size: 1em;
      background-color: #28a745;
      color: white;
      border: none;
      border-radius: 6px;
      margin-bottom: 10px;
      cursor: pointer;
      transition: background-color 0.3s;
    }

    .modal-body button:not(.blue-button):hover {
      background-color: #218838;
    }

    .modal-footer {
      display: flex;
      justify-content: flex-end;
      gap: 10px;
      margin-top: 10px;
    }

    /* Spécifique aux boutons verts dans Debug pour Reset */
    .reset-btn {
      width: 48%;
      padding: 10px;
      font-size: 1em;
      background-color: #28a745;
      color: white;
      border: none;
      border-radius: 6px;
      cursor: pointer;
      transition: background-color 0.3s;
      margin-top: 10px;
    }

    .reset-btn:hover {
      background-color: #218838;
    }

    /* Pour les boutons verts de même taille et espacés dans modal-body */
    .modal-body > button:not(.blue-button) {
      display: block;
      width: 100%;
      margin-bottom: 12px;
      box-sizing: border-box;
    }

    /* Classe pour forcer bleu sur boutons ferm/Envoyer */
    .blue-button {
      background-color: #007bff !important;
      color: white !important;
      margin: 0 !important;
    }

    .blue-button:hover {
      background-color: #0069d9 !important;
      transform: scale(1.05);
    }

    .blue-button:active {
      background-color: #005cbf !important;
      transform: scale(1);
    }

    .overlay {
      display: none;
      position: fixed;
      top: 0; left: 0;
      width: 100%;
      height: 100%;
      background-color: rgba(0, 0, 0, 0.5);
      z-index: 999;
    }
    #Joy{
      display: none;
    }
  </style>
</head>
<body>
  <h1>ESP32 - DrawBOT</h1>
  <div id="Joy">
    <div id="joystickZone">
      <div id="stick"></div>
    </div>

    <div id="values" style="text-align: center;">
      X: <span id="xVal">0</span> |
      Y: <span id="yVal">0</span>
    </div>
  </div>

  <h2><b>Options DrawBOT</b></h2>

  <div class="button-container">
    <button onclick="openEscalierModal()">Escalier</button>
    <button onclick="openCircleModal()">Cercle</button>
    <button onclick="openRoseModal()">Rose des Vents</button>
    <button onclick="openDebugModal()">Debug</button>
    <button onclick="toogleJoy()">Joystik On/Off</button>
  </div>

  <!-- Overlay -->
  <div class="overlay" id="overlay"></div>

  <!-- Modal Cercle -->
  <div id="circleModal" class="modal">
    <div class="modal-header">Paramètres du Cercle</div>
    <div class="modal-body">
      <label>Rayon (en mm): <input type="number" id="circleRadius" value="100" min="1"></label><br>
      <label>Nombre de points : <input type="number" id="circlePoints" value="100" min="1" step="1"></label><br>
      <div style="display: flex; align-items: center; gap: 8px;">
        <label>Rosace:</label>
        <input type="checkbox" id="circleRosace">
      </div>
    </div>
    <div class="modal-footer">
      <button class="blue-button" onclick="closeCircleModal()">Fermer</button>
      <button class="blue-button" onclick="sendCircleParams()">Envoyer</button>
    </div>
  </div>


  <!-- Modal Debug -->
  <div id="debugModal" class="modal">
    <div class="modal-header">Paramètres PID</div>
    <div class="modal-body">
      <label>KP angle: <input type="number" id="kp_ang" value="1.0" step="0.1" /></label><br />
      <label>KI angle: <input type="number" id="ki_ang" value="0.0" step="0.1" /></label><br />
      <label>KD angle: <input type="number" id="kd_ang" value="0.0" step="0.1" /></label><br />
      <label>KP distance: <input type="number" id="kp_dist" value="1.0" step="0.1" /></label><br />
      <label>KI distance: <input type="number" id="ki_dist" value="0.0" step="0.1" /></label><br />
      <label>KD distance: <input type="number" id="kd_dist" value="0.0" step="0.1" /></label><br />

      <button class="reset-btn" onclick="resetPidAngle()">Reset PID Angle</button>
      <button class="reset-btn" onclick="resetPidDistance()">Reset PID Distance</button>
    </div>
    <div class="modal-footer">
      <button class="blue-button" onclick="closeDebugModal()">Fermer</button>
      <button class="blue-button" onclick="sendDebugParams()">Envoyer</button>
    </div>
  </div>

  <!-- Modal Rose des Vents -->
  <div id="roseModal" class="modal">
    <div class="modal-header">Séquence Rose des Vents</div>
    <div class="modal-body">
      <button onclick="sendRoseSequence('windrose')">Tracer une rose des vents</button>
      <button onclick="sendRoseSequence('arrow')">Tracer une flèche</button>
    </div>
    <div class="modal-footer">
      <button class="blue-button" onclick="closeRoseModal()">Fermer</button>
    </div>
  </div>

  <!-- Modal Escalier -->
  <div id="escalierModal" class="modal">
    <div class="modal-header">Séquence Escalier</div>
    <div class="modal-body">
      <button onclick="sendSequence('step')">Tracer un escalier</button>
      <label>Nombre de carrés :</label><br>
      <input type="number" id="escalierNb" value ="4" step="1"><br>
      <label>Longueur du côté du carré le plus grand (en mm) :</label><br>
      <input type="number" id="escalierCote" value ="100" step="1"><br>
    </div>
    <div class="modal-footer">
      <button class="blue-button" onclick="sendSequence('multiCarre')">Envoyer</button>
      <button class="blue-button" onclick="closeEscalierModal()">Fermer</button>
    </div>
  </div>

  <script>
    const zone = document.getElementById("joystickZone");
    const stick = document.getElementById("stick");
    const xVal = document.getElementById("xVal");
    const yVal = document.getElementById("yVal");

    let centerX = zone.offsetWidth / 2;
    let centerY = zone.offsetHeight / 2;
    let maxRange = 150;

    zone.addEventListener("touchmove", function (event) {
      event.preventDefault();
      let touch = event.touches[0];
      let rect = zone.getBoundingClientRect();
      let x = touch.clientX - rect.left - centerX;
      let y = touch.clientY - rect.top - centerY;

      const dist = Math.sqrt(x * x + y * y);
      if (dist > maxRange) {
        x *= maxRange / dist;
        y *= maxRange / dist;
      }

      stick.style.left = x + centerX - 40 + "px";
      stick.style.top = y + centerY - 40 + "px";

      let mappedX = Math.round((x / maxRange) * 255);
      let mappedY = Math.round((y / maxRange) * 255);

      xVal.innerText = mappedX;
      yVal.innerText = mappedY;

      fetch(`/joy?x=${mappedX}&y=${mappedY}`).catch((e) => {});
    });

    zone.addEventListener("touchend", () => {
      stick.style.left = "110px";
      stick.style.top = "110px";
      xVal.innerText = 0;
      yVal.innerText = 0;
      fetch(`/joy?x=0&y=0`).catch((e) => {});
    });

    //Escalier
    function sendSequence(seq) { //seq = multiCarre ou step
      const nb_carre = document.getElementById("escalierNb").value;
      const coter = document.getElementById("escalierCote").value;
      fetch(`/sequence?type=${seq}&nb_carre=${nb_carre}&coter=${coter}`).catch((e) => {});
      if(seq=="multiCarre") closeEscalierModal();
    }

    function sendRoseSequence(seq) {
      fetch(`/rose?type=${seq}`).catch((e) => {});
    }

    // Modales open/close
    const overlay = document.getElementById("overlay");

    function openModal(modal) {
      modal.style.display = "block";
      overlay.style.display = "block";
    }

    function closeModal(modal) {
      modal.style.display = "none";
      overlay.style.display = "none";
    }

    // Cercle
    const circleModal = document.getElementById("circleModal");
    function openCircleModal() {
      openModal(circleModal);
    }
    function closeCircleModal() {
      closeModal(circleModal);
    }
    function sendCircleParams() {
      const r = document.getElementById("circleRadius").value;
      const points = document.getElementById("circlePoints").value;
      const rosace = document.getElementById("circleRosace").checked ? 1 : 0;
      let type ;
      if(rosace){
         type = "Rosace"; 
      } else{
         type = "Circle"; 
      }

      fetch(`/circle?radius=${r}&points=${points}&type=${type}`).catch((e) => {});
      closeCircleModal();
    }


    // Debug
    const debugModal = document.getElementById("debugModal");
    function openDebugModal() {
      openModal(debugModal);
    }
    function closeDebugModal() {
      closeModal(debugModal);
    }
    function sendDebugParams() {
      const kp_ang = document.getElementById("kp_ang").value;
      const ki_ang = document.getElementById("ki_ang").value;
      const kd_ang = document.getElementById("kd_ang").value;
      const kp_dist = document.getElementById("kp_dist").value;
      const ki_dist = document.getElementById("ki_dist").value;
      const kd_dist = document.getElementById("kd_dist").value;

      fetch(
        `/pid?kp_ang=${kp_ang}&ki_ang=${ki_ang}&kd_ang=${kd_ang}&kp_dist=${kp_dist}&ki_dist=${ki_dist}&kd_dist=${kd_dist}`
      ).catch((e) => {});

      closeDebugModal();
    }

    // Reset PID Angle
    function resetPidAngle() {
      document.getElementById("kp_ang").value = 1.0;
      document.getElementById("ki_ang").value = 0.0;
      document.getElementById("kd_ang").value = 0.0;
    }

    // Reset PID Distance
    function resetPidDistance() {
      document.getElementById("kp_dist").value = 1.0;
      document.getElementById("ki_dist").value = 0.0;
      document.getElementById("kd_dist").value = 0.0;
    }

    // Rose
    const roseModal = document.getElementById("roseModal");
    function openRoseModal() {
      openModal(roseModal);
    }
    function closeRoseModal() {
      closeModal(roseModal);
    }

    // Escalier
    const escalierModal = document.getElementById("escalierModal");
    function openEscalierModal() {
      openModal(escalierModal);
    }
    function closeEscalierModal() {
      closeModal(escalierModal);
    }

    overlay.addEventListener("click", () => {
      // Ferme toutes les modales si on clique sur overlay
      closeCircleModal();
      closeDebugModal();
      closeRoseModal();
      closeEscalierModal();
    });

    function toogleJoy(){
      const joyDiv = document.getElementById("Joy");
      if (joyDiv.style.display === "none") {
        joyDiv.style.display = "block"; // Affiche la div
      } else {
        joyDiv.style.display = "none"; // Cache la div
      }      
    }
  </script>
</body>
</html>


  <!-- Fin du HTML-->

  )rawliteral";
    