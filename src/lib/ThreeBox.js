const prefix = (key) => `ifpsUploader.${key}`

const setFiles = (box, callback) => value =>
  box.private.set(prefix('files'), value)
    .then(res => (res && typeof callback === 'function' && callback()))

const getFiles = (box) =>
  box.private.get(prefix('files'))
    .then(JSON.parse)

const setFileProperty = (box, ipfsHash, name, value) =>
  box.private.set(prefix(`files[${ipfsHash}].${name}`), value)

const setFileName = (box, ipfsHash, fileName) => setFileProperty(box, ipfsHash, 'name', fileName)

const setFileDate = (box, ipfsHash) => setFileProperty(box, ipfsHash, 'date', Date.now().toString())

const addFile = (box, ipfsHash, callback) =>
  getFiles(box)
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

export {
  setFileName,
  setFileDate,
  addFile,
  getFiles
}