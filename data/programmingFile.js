async function start() {
  await node.executeInstruction(location.host, {
    action: "pinMode",
    parameters: [2, "OUTPUT"]
  });

  let outputState = false;
  let latencyArray = [];

  for (let i = 0; i < 100; i++) {
    let timeStart = Date.now();
    outputState = !outputState;
    if (outputState) {
      await node.executeInstruction(location.host, {
        action: "digitalWrite",
        parameters: [2, "HIGH"]
      });
    } else {
      await node.executeInstruction(location.host, {
        action: "digitalWrite",
        parameters: [2, "LOW"]
      });
    }
    let timeEnd = Date.now();
    let latency = timeEnd - timeStart;
    latencyArray.push(latency);

    console.log(JSON.stringify({ iteration: i, latency: latency }));
  }

  let sum = 0;
  for (let i of latencyArray) {
    sum += i;
  }
  let latencyAverage = sum / latencyArray.length;
  let instructionsPerSecond = (1000 / latencyAverage).toFixed(2);
  let platform = navigator.platform;
  console.log(
    JSON.stringify({ latencyAverage, instructionsPerSecond, platform })
  );
}
start();
