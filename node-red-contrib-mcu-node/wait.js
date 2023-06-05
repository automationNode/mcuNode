module.exports = function (RED) {
  function wait(config) {
    RED.nodes.createNode(this, config);
    var node = this;
    let delayActivated = undefined;
    let delayAbort = false;
    let interval = undefined;
    node.on("close", function () {
      delayAbort = true;
    });
    node.on("input", async (msg) => {
      console.log("-->Input:", config);
      if (msg.payload.executed == true || msg.payload.start == true) {
        delayActivated = await node.context().flow.get("delayActivated");
        if(delayActivated == undefined){
          await node.context().flow.set("delayActivated", false);
          delayActivated = await node.context().flow.get("delayActivated");
        }
        async function _wait(time) {
          return new Promise((resolve) => {
            setTimeout(() => {
              clearInterval(interval);
              resolve(true);
            }, time);
            let timeTranscurred = 0;
            let interval = setInterval(() => {
              timeTranscurred += 50 * 1.25;
              if (delayAbort) {
                clearInterval(interval);
                delayAbort = false;
                resolve(true);
              } else {
                node.status({
                  fill: "yellow",
                  shape: "dot",
                  text: `Waiting... ${timeTranscurred} ms`,
                });
              }
            }, 50);
          });
        }
        if (!delayActivated) {
          
          await node.context().flow.set("delayActivated", true);
          delayActivated = await node.context().flow.get("delayActivated");

          console.time("TranscurredTime");
          await _wait(Number(config.timeMs));
          console.timeEnd("TranscurredTime");

          await node.context().flow.set("delayActivated", false);
          delayActivated = await node.context().flow.get("delayActivated");

          node.status({ fill: "green", shape: "dot", text: "Executed" });
          let output = { payload: { executed: true } };
          console.log("-->Output:", output);
          node.send(output);
        }
      }
    });
  }
  RED.nodes.registerType("wait", wait);
};
