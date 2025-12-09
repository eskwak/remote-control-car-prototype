/**
 * Description:       JS for vehicle controller interface.
 * 
 * Author:            Eddie Kwak
 * Last Modified:     12/8/2025
 *  */ 

// make sure to update this with ESP32's current address
const ESP_ADDRESS = "http://192.168.137.139";

// states
let speed = 80;
let forward = true;
let turn = 0; // left = -1, straight = 0, right = 1
let accelerating = false;

// UI helpers
const dirLabel = document.getElementById("dirLabel");
const speedLabel = document.getElementById("speedLabel");
const turnLabel = document.getElementById("turnLabel");
const accelLabel = document.getElementById("accelLabel");

// updates UI labels to reflect current controls settings
function updateUI() {
  dirLabel.textContent = forward ? "FORWARD" : "REVERSE";
  speedLabel.textContent = speed.toString();

  if (turn === -1) {
    turnLabel.textContent = "LEFT";
  }
  else if (turn === 1) {
    turnLabel.textContent = "RIGHT";
  }
  else {
    turnLabel.textContent = "STRAIGHT";
  }
  accelLabel.textContent = accelerating ? "YES" : "NO";
}

// sends current state to ESP32 webserver
function pushState() {
  const params = new URLSearchParams({
    speed: speed.toString(),
    forward: forward ? "1" : "0",
    turn: turn.toString(),                // -1, 0, 1
    accelerating: accelerating ? "1" : "0"
  });

  fetch(`${ESP_ADDRESS}/control?${params.toString()}`)
    .catch(console.error);
}


// increase speed button
document.getElementById("speedUpBtn").onclick = () => {
  speed = Math.min(100, speed + 10);
  updateUI();
  pushState();
};

// decrease speed button
document.getElementById("speedDownBtn").onclick = () => {
  speed = Math.max(60, speed - 10);
  updateUI();
  pushState();
};

// when movement button is pressed
function startMove(isForward, turnValue) {
  forward = isForward;
  turn = turnValue;
  accelerating = true;
  updateUI();
  pushState();
}

// when movement button is released
function stopMove() {
  accelerating = false;
  updateUI();
  pushState();
}

// web/mobile buttotn handlers for movement
function bindHoldButton(button, isForward, turnValue) {
  // mouse
  button.addEventListener("mousedown", () => startMove(isForward, turnValue));
  button.addEventListener("mouseup", stopMove);
  button.addEventListener("mouseleave", stopMove);

  // mobile 
  button.addEventListener("touchstart", e => {
    e.preventDefault();
    startMove(isForward, turnValue);
  });
  button.addEventListener("touchend", e => {
    e.preventDefault();
    stopMove();
  });
}

// direction buttons
const forwardBtn = document.getElementById("forwardBtn");
const backwardBtn = document.getElementById("backwardBtn");
const leftBtn = document.getElementById("leftBtn");
const rightBtn = document.getElementById("rightBtn");

// bind buttons
bindHoldButton(forwardBtn, true, 0);
bindHoldButton(backwardBtn, false, 0);
bindHoldButton(leftBtn, true, -1);
bindHoldButton(rightBtn, true, 1);

updateUI();
pushState();