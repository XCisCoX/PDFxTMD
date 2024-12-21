#pragma once
#include "Common/ConfigWrapper.h"
#include "Common/Exception.h"
#include "Common/PDFUtils.h"
#include "Common/PartonUtils.h"
#include "Common/YamlInfoReader.h"
#include "Interface/IExtrapolator.h"
#include "Interface/IInterpolator.h"
#include "Interface/IReader.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace PDFxTMD
{
struct TMDPDFTag
{
};
struct CollinearPDFTag
{
};

template <typename Tag, typename Reader, typename Interpolator, typename Extrapolator>
class GenericPDF
{
  public:
    GenericPDF(const std::string &pdfName, int setNumber)
        : m_pdfName(pdfName), m_setNumber(setNumber)
    {
        initializeComponents();
        loadStandardInfo();
    }
    ~GenericPDF()
    {
    }
    double pdf(PartonFlavor flavor, double x, double mu2)
    {
        if constexpr (std::is_same_v<Tag, CollinearPDFTag>)
        {
            return computeCollinearPDF(flavor, x, mu2);
        }
        else
        {
            throw std::logic_error(
                "pdf(PartonFlavor, double, double) is not supported for this tag.");
        }
    }

    double tmd(PartonFlavor flavor, double x, double kt2, double mu2)
    {
        if constexpr (std::is_same_v<Tag, TMDPDFTag>)
        {
            return computeTMD(flavor, x, kt2, mu2);
        }
        else
        {
            throw std::logic_error(
                "pdf(PartonFlavor, double, double) is not supported for this tag.");
        }
    }

    YamlStandardTMDInfo getStdPDFInfo() const
    {
        return m_stdInfo;
    }

  private:
    // Initialization and loading functions
    void initializeComponents()
    {
        if constexpr (std::is_base_of_v<IAdvancedExtrapolator<Extrapolator, Reader, Interpolator>,
                                        Extrapolator>)
        {
            m_extrapolator->setInterpolator(m_interpolator);
        }
    }
    void loadStandardInfo()
    {
        auto infoPathPair = StandardInfoFilePath(m_pdfName);
        if (infoPathPair.second != ErrorType::None)
            throw FileLoadException("Unable to find info file of PDF set " + m_pdfName);
        if constexpr (std::is_same_v<Tag, TMDPDFTag>)
        {
            auto pdfStandardInfo =
                YamlStandardPDFInfoReader<YamlStandardTMDInfo>(*infoPathPair.first);
            if (pdfStandardInfo.second != ErrorType::None)
                throw InvalidFormatException("Invalid standard info file " + *infoPathPair.first);
            m_stdInfo = *pdfStandardInfo.first;
        }
        else if constexpr (std::is_same_v<Tag, CollinearPDFTag>)
        {
            auto pdfStandardInfo =
                YamlStandardPDFInfoReader<YamlStandardPDFInfo>(*infoPathPair.first);
            if (pdfStandardInfo.second != ErrorType::None)
                throw InvalidFormatException("Invalid standard info file " + *infoPathPair.first);
            m_stdInfo = *pdfStandardInfo.first;
        }
        else
        {
            static_assert(!std::is_same_v<Tag, Tag>, "Unsupported Tag");
        }
    }
    double computeCollinearPDF(PartonFlavor flavor, double x, double mu)
    {
        if (!m_dataLoaded)
        {
            loadData();
        }
        if (!isInRange(m_reader, x, mu))
        {
            return m_extrapolator.extrapolate(&m_reader, flavor, x, mu);
        }
        return m_interpolator.interpolate(flavor, x, mu);
    }

    double computeTMD(PartonFlavor flavor, double x, double kt, double mu)
    {
        if (!m_dataLoaded)
        {
            loadData();
        }
        if (!isInRange(m_reader, x, kt, mu))
        {
            return m_extrapolator.extrapolate(&m_reader, flavor, x, kt, mu);
        }
        return m_interpolator.interpolate(flavor, x, kt, mu);
    }

    void loadData()
    {
        try
        {
            m_reader.read(m_pdfName, m_setNumber);
            m_interpolator.initialize(&m_reader);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error reading PDF data: " << e.what() << std::endl;
            throw;
        }
        m_dataLoaded = true;
    }

    std::string m_pdfName;
    int m_setNumber;
    Reader m_reader;
    Interpolator m_interpolator;
    Extrapolator m_extrapolator;
    bool m_dataLoaded = false;
    YamlStandardTMDInfo m_stdInfo;
};
} // namespace PDFxTMD