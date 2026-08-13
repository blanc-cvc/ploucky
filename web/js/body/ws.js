


// BASE: Worker, fallback to callback if !window.Worker
const onmessage_common = (onmessage_common_e, onmessage_common_cb_fn = false) => {
  // onmessage_common_e is an object event containing data (as worker)
  //                    is an object just containing data (as callback)
  
  
}
// without Worker:
//   call require(_this_)._onmessage with postMessage_data
//   and use _onmessage_cb_fn as callback
exports._onmessage = (_onmessage_e, _onmessage_cb_fn) => {
  onmessage_common(_onmessage_e, (data) => {
    _onmessage_cb_fn(data);
  });
}

onmessage = onmessage_common;

