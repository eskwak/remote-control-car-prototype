/**
 * Description:       JS for vehicle controller interface.
 * 
 * Author:            Eddie Kwak
 * Last Modified:     12/8/2025
 *  */ 

// make sure to update this with ESP32's current address
const ESP_ADDRESS = "http://192.168.137.43";

// states
let speed = 5;
let forward = true;
let turn = 0; // left = -1, straight = 0, right = 1
let accelerating = false;
let autonomous = false;

// track which buttons are currently pressed
let forwardPressed = false;
let backwardPressed = false;
let leftPressed = false;
let rightPressed = false;

// UI helpers
const dirLabel = document.getElementById("dirLabel");
const speedLabel = document.getElementById("speedLabel");
const turnLabel = document.getElementById("turnLabel");
const accelLabel = document.getElementById("accelLabel");
const autoBtn = document.getElementById("autoBtn");

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
    turn: turn.toString(),
    accelerating: accelerating ? "1" : "0",
    auto: autonomous ? "1" : "0"
  });

  fetch(`${ESP_ADDRESS}/control?${params.toString()}`)
    .catch(console.error);
}

function updateAutoButton() {
  autoBtn.textContent = autonomous ? "Auto: ON" : "Auto: OFF";
}

autoBtn.onclick = () => {
  autonomous = !autonomous;

  if (autonomous) {
    accelerating = false;
    forward = true;
    turn = 0;
    forwardPressed = false;
    backwardPressed = false;
    leftPressed = false;
    rightPressed = false;
  }

  updateUI();
  updateAutoButton();
  pushState();
};

// increase speed button
document.getElementById("speedUpBtn").onclick = () => {
  speed = Math.min(80, speed + 5);
  updateUI();
  pushState();
};

// decrease speed button
document.getElementById("speedDownBtn").onclick = () => {
  speed = Math.max(5, speed - 5);
  updateUI();
  pushState();
};

// when movement button is pressed
function startMove(isForward) {
  if (autonomous) return;

  if (isForward) {
    forwardPressed = true;
  } else {
    backwardPressed = true;
  }
  forward = isForward;
  accelerating = true;
  updateUI();
  pushState();
}

// when movement button is released
function stopMove(isForward) {
  if (isForward) {
    forwardPressed = false;
  } else {
    backwardPressed = false;
  }
  // Only stop accelerating if no movement buttons are pressed
  if (!forwardPressed && !backwardPressed) {
    accelerating = false;
  } else {
    // Update direction to the still-pressed button
    forward = forwardPressed;
  }
  updateUI();
  pushState();
}

// when left/right button is pressed
function startTurn(turnValue) {
  if (autonomous) return;

  if (turnValue === -1) {
    leftPressed = true;
  } else {
    rightPressed = true;
  }
  turn = turnValue;
  updateUI();
  pushState();
}

// when left/right button is released
function stopTurn(turnValue) {
  if (turnValue === -1) {
    leftPressed = false;
  } else {
    rightPressed = false;
  }
  // Only reset turn if no turn buttons are pressed
  if (!leftPressed && !rightPressed) {
    turn = 0;
  } else {
    // Keep the still-pressed turn direction
    turn = leftPressed ? -1 : 1;
  }
  updateUI();
  pushState();
}

// button handlers for forward/backward movement
function bindDriveButton(button, isForward) {
  // mouse
  button.addEventListener("mousedown", () => startMove(isForward));
  button.addEventListener("mouseup", () => stopMove(isForward));
  button.addEventListener("mouseleave", () => stopMove(isForward));

  // mobile
  button.addEventListener("touchstart", e => {
    e.preventDefault();
    startMove(isForward);
  });
  button.addEventListener("touchend", e => {
    e.preventDefault();
    stopMove(isForward);
  });
}

// button handlers for turning
function bindTurnButton(button, turnValue) {
  // mouse
  button.addEventListener("mousedown", () => startTurn(turnValue));
  button.addEventListener("mouseup", () => stopTurn(turnValue));
  button.addEventListener("mouseleave", () => stopTurn(turnValue));

  // mobile
  button.addEventListener("touchstart", e => {
    e.preventDefault();
    startTurn(turnValue);
  });
  button.addEventListener("touchend", e => {
    e.preventDefault();
    stopTurn(turnValue);
  });
}

// direction buttons
const forwardBtn = document.getElementById("forwardBtn");
const backwardBtn = document.getElementById("backwardBtn");
const leftBtn = document.getElementById("leftBtn");
const rightBtn = document.getElementById("rightBtn");

// bind forward/backward buttons
bindDriveButton(forwardBtn, true);
bindDriveButton(backwardBtn, false);

// bind turning buttons
bindTurnButton(leftBtn, -1);
bindTurnButton(rightBtn, 1);

updateUI();
pushState();
updateAutoButton();