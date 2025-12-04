// app.js
import { initializeApp } from "https://www.gstatic.com/firebasejs/11.0.1/firebase-app.js";
import {
  getDatabase,
  ref,
  set,
  onValue,
} from "https://www.gstatic.com/firebasejs/11.0.1/firebase-database.js";

// ---- 1) Firebase config (yours) ----
const firebaseConfig = {
  apiKey: "AIzaSyBGXfmSrV504OtG8232OJ1NKeNPC2y6s3o",
  authDomain: "autonomous-vehicle-985f1.firebaseapp.com",
  databaseURL: "https://autonomous-vehicle-985f1-default-rtdb.firebaseio.com",
  projectId: "autonomous-vehicle-985f1",
  storageBucket: "autonomous-vehicle-985f1.firebasestorage.app",
  messagingSenderId: "486094215958",
  appId: "1:486094215958:web:7a031f6af8bc88c8fc8e37",
  measurementId: "G-100C0RMZR8",
};

// ---- 2) Init Firebase ----
const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

const directionRef = ref(db, "car/direction");
const speedRef = ref(db, "car/speed");

// we keep this mirrored with RTDB
let currentSpeed = 0.4;

// ---- 3) UI helpers ----
const statusEl = document.getElementById("status");
const dirDisplay = document.getElementById("dir-display");
const speedDisplay = document.getElementById("speed-display");

function setStatus(msg) {
  if (statusEl) statusEl.textContent = msg;
}

function clampSpeed(x) {
  if (x < 0.2) return 0.2;
  if (x > 0.8) return 0.8;
  return x;
}

// ---- 4) Button wiring ----
document.addEventListener("DOMContentLoaded", () => {
  document
    .getElementById("btn-forward")
    .addEventListener("click", () => set(directionRef, "forward"));
  document
    .getElementById("btn-back")
    .addEventListener("click", () => set(directionRef, "back"));
  document
    .getElementById("btn-stop")
    .addEventListener("click", () => set(directionRef, "stop"));

  document.getElementById("btn-faster").addEventListener("click", async () => {
    currentSpeed = clampSpeed(currentSpeed + 0.2);
    await set(speedRef, currentSpeed);
  });

  document.getElementById("btn-slower").addEventListener("click", async () => {
    currentSpeed = clampSpeed(currentSpeed - 0.2);
    await set(speedRef, currentSpeed);
  });
});

// ---- 5) Live updates from RTDB ----
onValue(directionRef, (snap) => {
  const dir = snap.val() ?? "stop";
  if (dirDisplay) dirDisplay.textContent = dir;
  setStatus(`Direction: ${dir}, speed: ${currentSpeed.toFixed(2)}`);
});

onValue(speedRef, (snap) => {
  const val = snap.val();
  if (typeof val === "number") {
    currentSpeed = clampSpeed(val);
  }
  if (speedDisplay) speedDisplay.textContent = currentSpeed.toFixed(2);
  setStatus(`Direction: ${dirDisplay.textContent}, speed: ${currentSpeed.toFixed(2)}`);
});
