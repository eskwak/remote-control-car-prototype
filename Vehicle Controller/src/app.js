/**
 * Description:       JS module for vehicle controller interface. Handles Firebase 
 *                    configuration, states, UI updates, and writing vehicle control 
 *                    commands to the firebase RTDB.
 * 
 * Author:            Eddie Kwak
 * Last Modified:     12/7/2025
 *  */ 

// firebase imports
import { initializeApp } from "https://www.gstatic.com/firebasejs/11.0.0/firebase-app.js";
import {
  getDatabase,
  ref,
  update
} from "https://www.gstatic.com/firebasejs/11.0.0/firebase-database.js";

const firebaseConfig = {
  
}

const app = initializeApp(firebaseConfig);
const db = getDatabase(app);
const controlRef = ref(db, "car/control");

// states
let speed = 50;
let forward = true;
let turn = 0;
let accelerating = false;

// UI helpers
const dirLabel = document.getElementById("dirLabel");
const speedLabel = document.getElementById("speedLabel");
const turnLabel = document.getElementById("turnLabel");
const accelLabel = document.getElementById("accelLabel");

// update UI labels to reflect current controls
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

// push current state to RTDB
function pushState() {
  update(controlRef, {speed, forward, turn, accelerating}).catch(err => console.error(err));
}

// toggle between forward and reverse direction
document.getElementById("dirBtn").onclick = () => {
  forward = !forward;
  updateUI();
  pushState();
};

// increase speed
document.getElementById("speedUpBtn").onclick = () => {
  speed = Math.min(100, speed + 10);
  updateUI();
  pushState();
};

// decrease speed
document.getElementById("speedDownBtn").onclick = () => {
  speed = Math.max(50, speed - 10);
  updateUI();
  pushState();
};

// set turn direction to left
document.getElementById("leftBtn").onclick = () => {
  turn = -1;
  updateUI();
  pushState();
};

// set turn direction to right
document.getElementById("rightBtn").onclick = () => {
  turn = 1;
  updateUI();
  pushState();
};

// set turn direction to straight
document.getElementById("straightBtn").onclick = () => {
  turn = 0;
  updateUI();
  pushState();
};

// enable/disable accelerating
function setAccelerating(val) {
  accelerating = val;
  updateUI();
  pushState();
}

const accelBtn = document.getElementById("accelerateBtn");

// events for accelerating (for web testing)
accelBtn.addEventListener("mousedown", () => setAccelerating(true));
accelBtn.addEventListener("mouseup", () => setAccelerating(false));
accelBtn.addEventListener("mouseleave", () => setAccelerating(false));

// events for accelerating (on phone)
accelBtn.addEventListener("touchstart", e => {
  e.preventDefault();
  setAccelerating(true);
});

accelBtn.addEventListener("touchend", e => {
  e.preventDefault();
  setAccelerating(false);
});

updateUI();
pushState();