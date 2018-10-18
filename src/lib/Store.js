const prefixKey = key => 'ipfsUploader/' + key

// persisteFile = { name: string, hash: string }
export const storeFile = (box, ipfsHash) =>
  box.private.get(prefixKey('files')).then(files => {
    const newFiles = Array.isArray(files) ? files : []
    newFiles.push({ name: '', hash: ipfsHash, date: Date.now() })
    box.private
      .set(prefixKey('files'), newFiles)
      .then(result => console.log(result))
  })

export const fetchFiles = (box, callback) =>
  box.private
    .get(prefixKey('files'))
    .then(files => {
      if (!Array.isArray(files))
        return typeof callback === 'function' && callback([])
      typeof callback === 'function' && callback(files)
    })
    .catch(err => {
      console.error(err)
    })

export const saveFiles = (box, files, callback) =>
  box.private.set(prefixKey('files'), files).then(result => {
    console.log(result)
    fetchFiles(box, callback)
  })
