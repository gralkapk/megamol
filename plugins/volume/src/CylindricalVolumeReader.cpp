#include "CylindricalVolumeReader.h"

#include "mmcore/param/FilePathParam.h"

namespace megamol::volume {
CylindricalVolumeReader::CylindricalVolumeReader() : filename_slot_("filename", "") {
    this->filename_slot_.SetParameter(new core::param::FilePathParam(""));
    this->MakeSlotAvailable(&this->filename_slot_);
}

CylindricalVolumeReader::~CylindricalVolumeReader(){
    this->Release();
}

bool CylindricalVolumeReader::create() {
    return true;
}

void CylindricalVolumeReader::release() {}

core::view::AnimDataModule::Frame* CylindricalVolumeReader::constructFrame() const {
    return nullptr;
}

void CylindricalVolumeReader::loadFrame(Frame* frame, unsigned int idx) {}
} // namespace megamol::volume
