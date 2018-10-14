const prefixKey = key => 'ipfsUploader/' + key

// persisteFile = { name: string, hash: string }
export const storeFile = (box, ipfsHash) =>
  box.private.get(prefixKey('files')).then(files => {
    const newFiles = Array.isArray(files) ? files : []
    newFiles.push({ name: '', hash: ipfsHash })
    box.private
      .set(prefixKey('files'), newFiles)
      .then(result => console.log(result))
  })

export const fetchFiles = (box, callback) =>
  box.private.get(prefixKey('files')).then(files => {
    console.log(files)
    typeof callback === 'function' && callback(files)
  })
