const prefix = (key) => `ifpsUploader.${key}`

const setPrivateStore = (box, { name, value }) => box.private.set(name, value)

const setFiles = (box, callback) => value =>
  box.private.set(prefix('files'), value)
    .then(res => (res && typeof callback === 'function' && callback()))
  
const addFile = (box, ipfsHash, callback) =>
  box.private.get(prefix('files'))
    .then(JSON.parse)
    .then(files => {
      if (Array.isArray(files)) {
        const newFiles = files
        newFiles.push(ipfsHash)
        return newFiles
      } else {
        return []
      }
    })
    .then(JSON.stringify)
    .then(setFiles(box, callback))