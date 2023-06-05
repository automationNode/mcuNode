export async function wait(time) {
  return new Promise((resolve) => {
    setTimeout(() => {
      resolve();
    }, time);
  });
}

export async function executeInstruction(direction, instruction) {
  const controller = new AbortController();
  const timeoutId = setTimeout(async function () {
    controller.abort();
  }, 250);

  try {
    let execute = await fetch("http://" + direction + "/data", {
      signal: controller.signal,
      method: "post",
      body: JSON.stringify({
        execute: true,
        instruction,
      }),
    });

    clearTimeout(timeoutId);

    let response = await execute.json();
    if (response.return == true) {
      return response.content.instruction.return;
    } else {
      return false;
    }
  } catch {
    return false
  }
}

export async function fetchResource(direction, password, body) {
  body.system = {
    password: password,
  };

  let response = await fetch("http://" + direction + "/data", {
    method: "post",
    body: JSON.stringify(body),
  });

  return await response.json();
}
