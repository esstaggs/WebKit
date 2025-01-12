/*
 * Copyright (C) 2024 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#if USE(CF)

#import <wtf/RetainPtr.h>
#import <wtf/spi/cocoa/SecuritySPI.h>

namespace WebKit {

// For now, the only way to serialize/deserialize SecTrust objects is via
// SecTrustSerialize()/SecTrustDeserialize(). rdar://122051469

class CoreIPCSecTrust {
public:
    CoreIPCSecTrust(SecTrustRef trust)
        : m_trustData(adoptCF(SecTrustSerialize(trust, NULL)))
    {
    }

    CoreIPCSecTrust(RetainPtr<CFDataRef> data)
        : m_trustData(data)
    {
    }

    CoreIPCSecTrust(const IPC::DataReference& data)
        : m_trustData(data.empty() ? nullptr : adoptCF(CFDataCreate(kCFAllocatorDefault, data.data(), data.size())))
    {
    }

    RetainPtr<SecTrustRef> createSecTrust() const
    {
        if (!m_trustData)
            return nullptr;

        return adoptCF(SecTrustDeserialize(m_trustData.get(), NULL));
    }

    IPC::DataReference dataReference() const
    {
        if (!m_trustData)
            return { };

        CFDataRef data = m_trustData.get();
        return { CFDataGetBytePtr(data), static_cast<size_t>(CFDataGetLength(data)) };
    }

private:
    RetainPtr<CFDataRef> m_trustData;
};

} // namespace WebKit

#endif // USE(CF)
