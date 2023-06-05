import * as node from "./node.js";

export async function showProgrammingInterface(backendJson) {
  document.body.innerHTML = `
        <div style="width: 95%; border-radius: 10px; background-color: black; color: white; text-align: center; margin-left: 2.5%; margin-top: 2.5%; border-right: 6px solid rgb(0, 160, 255); border-bottom: 6px solid rgb(0, 160, 255);">
        <br>
        <div style="width: 95%; background-color: rgb(54, 54, 54); border-radius: 10px; margin-left: 2.5%; border-right: 3px solid rgb(0, 160, 255); border-bottom: 3px solid rgb(0, 160, 255);">
            <h2>Prueba del protocolo Firmata</h2>
            <input type="button" id="buttonRun" value="Correr"  style="background-color: rgb(255, 117, 37); color: white; border-radius: 10px; border-right: 3px solid rgb(0, 160, 255); border-bottom: 3px solid rgb(0, 160, 255);">
            <input type="button" id="buttonSave" value="Guardar"  style="background-color: rgb(0, 0, 200); color: white; border-radius: 10px; border-right: 3px solid rgb(0, 160, 255); border-bottom: 3px solid rgb(0, 160, 255);">
            <input type="button" id="buttonConfiguration" value="Configuration"  style="background-color: rgb(0, 255, 255); color: white; border-radius: 10px; border-right: 3px solid rgb(0, 160, 255); border-bottom: 3px solid rgb(0, 160, 255);">
            <br>
            <br>
        </div>
        <br>
        <textarea id="textareaProgramming" rows="18" style="resize: vertical; width: 95%; background-color: rgb(54, 54, 54); color: white; border-right: 3px solid rgb(0, 160, 255); border-bottom: 3px solid rgb(0, 160, 255);" spellcheck="false"></textarea>
        <br>
        <br>
        <textarea id="textareaDebugging" rows="8" readonly style="resize: vertical; width: 95%; background-color: rgb(54, 54, 54); color: white; border-right: 3px solid rgb(0, 160, 255); border-bottom: 3px solid rgb(0, 160, 255);"></textarea>
        <br>
        <br>
        </div>
    `;
  let buttonRun = document.getElementById("buttonRun");
  let buttonSave = document.getElementById("buttonSave");
  let buttonConfiguration = document.getElementById("buttonConfiguration");
  let textareaProgramming = document.getElementById("textareaProgramming");
  let textareaDebugging = document.getElementById("textareaDebugging");

  buttonConfiguration.addEventListener("click",async function () {
    showConfigurationInterface(backendJson);
  });

  function redirectConsole(_console) {
    // keep old log method
    var _oldConsoleLog = _console.log;
    _console.log = function (data) {
      textareaDebugging.value += `\n--> Log --> ${data}`;
      textareaDebugging.scrollTop = textareaDebugging.scrollHeight;
      _oldConsoleLog.call(_console, data);
    };

    // keep old error method
    var _oldConsoleError = _console.error;
    _console.error = function (data) {
      textareaDebugging.value += `\n--> Error --> ${data}`;
      textareaDebugging.scrollTop = textareaDebugging.scrollHeight;
      _oldConsoleError.call(_console, data);
    };

    // keep old warn method
    var _oldConsoleWarn = _console.warn;
    _console.warn = function (data) {
      textareaDebugging.value += `\n--> Warn --> ${data}`;
      textareaDebugging.scrollTop = textareaDebugging.scrollHeight;
      _oldConsoleWarn.call(_console, data);
    };
  }
  redirectConsole(console);

  let response = await node.fetchResource(
    location.hostname,
    backendJson.system.password,
    {
      get: {
        programmingFile: true,
      },
    }
  );
  console.log(response.return);
  if (response.return == true) {
    textareaProgramming.value = response.content;

    buttonRun.addEventListener("click", async function () {
      console.log("Running !!");
      try {
        await eval(textareaProgramming.value);
      } catch (error) {
        console.error(error);
      }
    });

    buttonSave.addEventListener("click", async function () {
      console.log("Saving !!");
      let response = await node.fetchResource(
        location.host,
        backendJson.system.password,
        {
          set: {
            programmingFile: true,
          },
          content: textareaProgramming.value,
        }
      );
      console.log(response);
      if (response.return == true) {
        console.log("Saved !!");
        textareaProgramming.value = response.content;
      } else {
        console.error("Not saved file!!");
      }
    });
  } else {
    textareaDebugging.value = `--> Error with the node fetching the programming file`;
    textareaDebugging.scrollTop = textareaDebugging.scrollHeight;
  }
}

export async function showConfigurationInterface(backendJson) {
  document.body.innerHTML = `
    <div style="width: 95%; border-radius: 10px; background-color: black; color: white; text-align: center; margin-left: 2.5%; margin-top: 2.5%; border-right: 6px solid rgb(0, 160, 255); border-bottom: 6px solid rgb(0, 160, 255);">
        <br>
        <div style="width: 95%; background-color: rgb(54, 54, 54); border-radius: 10px; margin-left: 2.5%; border-right: 3px solid rgb(0, 160, 255); border-bottom: 3px solid rgb(0, 160, 255);">
          <h2>Configuration</h2>
          <input type="button" id="buttonBack" value="Regresar"  style="background-color: rgb(0, 255, 0); color: white; border-radius: 10px; border-right: 3px solid rgb(0, 160, 255); border-bottom: 3px solid rgb(0, 160, 255);">
          <br>
          <br>
        </div>
        <div style="width: 95%; background-color: rgb(54, 54, 54); border-radius: 10px; margin-left: 2.5%; border-right: 3px solid rgb(0, 160, 255); border-bottom: 3px solid rgb(0, 160, 255);">
          <h2>WiFi Client</h2>
          <h3>Activated:</h3>
          <input type="checkbox" id="checkboxWifiActivated">
          <br>
          <h3>Name:</h3>
          <input type="text" id="textWifiName">
          <br>
          <h3>Password:</h3>
          <input type="text" id="textWifiPassword">
          <br>
          <h3>Ip mode:</h3>
          <input type="radio" name="ipMode" id="radioIpStatic" value="static"> Static
          <br>
          <input type="radio" name="ipMode" id="radioIpDynamic" value="dynamic"> Dynamic
          <br>
          <h3>Ip:</h3>
          <input type="number" id="numberIp0" min="0" max="255">.<input type="number" id="numberIp1" min="0" max="255">.<input type="number" id="numberIp2" min="0" max="255">.<input type="number" id="numberIp3" min="0" max="255">
          <br>
          <br>
        </div>
        <br>
        <input type="button" id="buttonSave" value="Guardar"  style="background-color: rgb(0, 0, 200); color: white; border-radius: 10px; border-right: 3px solid rgb(0, 160, 255); border-bottom: 3px solid rgb(0, 160, 255);">
        <br>
        <br>
        </div>
    `;
  let checkboxWifiActivated = document.getElementById("checkboxWifiActivated");
  let buttonBack = document.getElementById("buttonBack")
  let textWifiName = document.getElementById("textWifiName");
  let textWifiPassword = document.getElementById("textWifiPassword");
  let radioIpStatic = document.getElementById("radioIpStatic");
  let radioIpDynamic = document.getElementById("radioIpDynamic");
  let numberIp0 = document.getElementById("numberIp0");
  let numberIp1 = document.getElementById("numberIp1");
  let numberIp2 = document.getElementById("numberIp2");
  let numberIp3 = document.getElementById("numberIp3");
  let buttonSave = document.getElementById("buttonSave");

  checkboxWifiActivated.checked = backendJson.wifi.client.activated;
  textWifiName.value = backendJson.wifi.client.name;
  textWifiPassword.value = backendJson.wifi.client.password;
  radioIpDynamic.checked = backendJson.wifi.client.ipMode.dynamic;
  radioIpStatic.checked = backendJson.wifi.client.ipMode.static;
  numberIp0.value = backendJson.wifi.client.ip0;
  numberIp1.value = backendJson.wifi.client.ip1;
  numberIp2.value = backendJson.wifi.client.ip2;
  numberIp3.value = backendJson.wifi.client.ip3;

  buttonBack.addEventListener("click", function(){
     showProgrammingInterface(backendJson);
  })

  buttonSave.addEventListener("click",async function () {
    let response = await node.fetchResource(location.host, backendJson.system.password, {
      set: {
        wifi: {
          configuration: true,
        },
      },
      client: {
        name: textWifiName.value,
        password: textWifiPassword.value,
        activated: checkboxWifiActivated.checked,
        ipMode: {
          static: radioIpStatic.checked,
          dynamic: radioIpDynamic.checked,
        },
        ip0: numberIp0.value,
        ip1: numberIp1.value,
        ip2: numberIp2.value,
        ip3: numberIp3.value,
      },
    });

    console.log(response);
  });
}
