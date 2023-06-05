import * as node from "./node.js";
import * as interfaces from "./interfaces.js";

let textPassword = document.getElementById("textPassword")
let buttonEnter = document.getElementById("buttonEnter")
let divAdvice = document.getElementById("divAdvice")

let backendJson = {};

async function start() {
  buttonEnter.addEventListener("click", async function () {
    let response = await node.fetchResource(location.hostname, textPassword.value, {
      get: {
        backendJson: true,
      },
    });
    console.log(response)
    if(response.return == true){
      backendJson = response.content
      interfaces.showProgrammingInterface(backendJson)
    }else{
      divAdvice.innerHTML = `Wrong Password, please try again with another!!`
    }
  });

}
start();
